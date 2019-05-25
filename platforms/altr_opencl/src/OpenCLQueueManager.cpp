#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>

#ifdef NDEBUG
#define LOG_HEADER "OpenCLQueueManager"
#endif
#include <glog/logging.h>

#include "blaze/Task.h"
#include "blaze/TaskManager.h"
#include "blaze/altr_opencl/OpenCLKernelQueue.h"
#include "blaze/altr_opencl/OpenCLPlatform.h"
#include "blaze/altr_opencl/OpenCLQueueManager.h"

namespace blaze {

OpenCLQueueManager::OpenCLQueueManager(
    Platform* _platform,
    int _reconfig_timer):
  QueueManager(_platform),
  power_(true),
  reconfig_timer(_reconfig_timer)
{
  ocl_platform = dynamic_cast<OpenCLPlatform*>(platform);

  if (!ocl_platform) {
    DLOG(ERROR) << "Platform pointer type is not OpenCLPlatform";
    throw std::runtime_error("Cannot create OpenCLQueueManager");
  }

  DVLOG(2) << "Set FPGA reconfigure counter = " << _reconfig_timer;

  // start executor
  executors.create_thread(
      boost::bind(&OpenCLQueueManager::do_start, this));
}

OpenCLQueueManager::~OpenCLQueueManager() {
  // interrupt all executors
  executors.interrupt_all();
  power_ = false;
  executors.join_all();
  DVLOG(1) << "Stopped OpenCLQueueManager";
}

void OpenCLQueueManager::add(AccWorker &conf) {
  // call base class method first
  QueueManager::add(conf);

  // record config to a table
  //acc_conf_table_[conf.id()] = conf;

  // then setup kernel queue based on the parameters
  std::map<std::string, std::string> key_val;

  for (int i = 0; i < conf.param_size(); ++i) {
    key_val[conf.param(i).key()] = conf.param(i).value();
  }

  int num_kernels = 1;
  if (key_val.count("num_kernels")) {
    num_kernels = std::stoi(key_val["num_kernels"]);
  }
  key_val.erase("num_kernels");

  std::vector<ConfigTable_ptr> conf_list;

  for (int i = 0; i < num_kernels; ++i) {
    std::string kname_key = std::string("kernel_name[") + 
                            std::to_string(i) + 
                            std::string("]");
    if (!key_val.count(kname_key)) {
      if (num_kernels == 1) {
        if (!key_val.count("kernel_name")) {
          LOG_IF(ERROR, VLOG_IS_ON(1)) << "Missing kernel name "  
            << "for acc " << conf.id();

          throw invalidParam(__func__);
        }
        else {
          kname_key = "kernel_name";
        }
      }
      else {
        LOG_IF(ERROR, VLOG_IS_ON(1)) << "Missing kernel name[" 
          << i << "] for acc " << conf.id();
        throw invalidParam(__func__);
        
      }
    }
    std::string kernel_name = key_val[kname_key];
    key_val.erase(kname_key);

    // add configuration to kernel conf
    ConfigTable_ptr task_conf(new ConfigTable());
    task_conf->write_conf("kernel_name", kernel_name);
    conf_table_[kernel_name] = task_conf;

    // parse all conf for a specific kernel, if it exists
    for (auto it = key_val.cbegin(); it != key_val.cend(); ) {
      // check if key is prefix of kernel_name
      auto res = std::mismatch(kernel_name.begin(),
          kernel_name.end(), it->first.begin());

      std::string key;
      if (res.first == kernel_name.end()) {
        // the key is for kernel
        key.assign(res.second + 1, it->first.end()); 
        DVLOG(2) << "Add conf key: " << key 
                   << " to kernel " << kernel_name; 

        // add the config to table
        task_conf->write_conf(key, it->second);

        // delete the current key
        it = key_val.erase(it);
      }
      else {
        ++it;
      }
    }

    conf_list.push_back(task_conf);
    
    // add to kernel list
    // TODO: do we need to initialize vector first?
    kernel_table_[conf.id()].push_back(kernel_name);      
  }

  // pass all remaining configurations to all kernels
  if (!key_val.empty()) {
    for (auto p : key_val) {
      DVLOG(2) << "Add conf key: " << p.first;
      for (auto c : conf_list) {
        c->write_conf(p.first, p.second);
      }
    }  
  }

  // call routine function to setup all kernels for acc
  ocl_platform->changeProgram(conf.id());
  setup_kernels(conf.id());

  curr_acc_ = conf.id();
}

void OpenCLQueueManager::remove(std::string id) {
  DVLOG(1) << "Removing opencl queue for " << id;

  // calling parent remove for task manager removal
  QueueManager::remove(id);

  // remove kernel for acc:id
  remove_kernels(id);

}

void OpenCLQueueManager::start() {
  // do nothing since the executors are already started
  //DVLOG(1) << "FPGAQueue started";
}

void OpenCLQueueManager::do_start() {
  
  OpenCLPlatform* ocl_platform = dynamic_cast<OpenCLPlatform*>(platform);

  if (!ocl_platform) {
    DLOG(ERROR) << "Platform pointer incorrect";
    return;
  }
  VLOG(1) << "Start a executor for FPGAQueueManager";

  int retry_counter = 0;
  std::list<std::pair<std::string, TaskManager_ptr> > ready_queues;

  while (power_) {
    if (queue_table.empty()) {
      // no ready queues at this point, sleep and check again
      boost::this_thread::sleep_for(boost::chrono::milliseconds(10)); 
      continue;
    }
    else {
      boost::lock_guard<QueueManager> guard(*this);

      // here a round-robin policy is enforced
      // iterate through all task queues
      if (ready_queues.empty()) {
        for (auto q : queue_table) {
          if (!q.second->isEmpty()) {
            ready_queues.push_back(q);
          }
        }
      }
    }

    if (ready_queues.empty()) {
      // no ready queues at this point, sleep and check again
      boost::this_thread::sleep_for(boost::chrono::microseconds(1000)); 
      continue;
    }

    // select first queue
    std::string queue_name = ready_queues.front().first;
    TaskManager_ptr queue  = ready_queues.front().second;

    // switch bitstream for the selected queue
    try {
      // first need to remove the related kernels
      // TODO: this part cause trouble since we don't wait for queue to finish
      if (!curr_acc_.empty() && curr_acc_ != queue_name) {
        DVLOG(1) << "Removing kernels for acc: " << curr_acc_;
        remove_kernels(queue_name);

        // change program in OpenCLPlatform, so that Env is refreshed
        ocl_platform->changeProgram(queue_name);

        curr_acc_ = queue_name;

        // setup new kernels based on the new acc
        setup_kernels(queue_name);
      }
    }
    catch (std::runtime_error &e) {
      
      DLOG(ERROR) << "Programing bitstream failed";
      
      ocl_platform->removeQueue(queue_name);

      // remove queue_name from ready queue since it's already removed
      ready_queues.pop_front();

      // if setup program keeps failing, remove accelerator from queue_table 
      DLOG(ERROR) << "Failed to setup bitstream for " << queue_name
        << ": " << e.what() << ". Remove it from QueueManager.";

      continue;
    }

    // timer to wait for the queue to fill up again
    int counter = 0;
    while (counter < reconfig_timer) {

      Task* task;
      if (queue->popReady(task)) {
        VLOG(2) << "Scheduling one task from " << queue_name;

        // do scheduling
        while (!schedule(queue_name, task)) {
          boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
        }

        // reset the counter
        counter = 0;
      }
      else { 
        if (VLOG_IS_ON(1)) {
          DLOG_EVERY_N(INFO, 50) << "Queue " << queue_name 
                               << " empty for " << counter << "ms";
        }

        // start counter
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 

        counter++;
      }
    }
    // if the timer is up, switch to the next queue
    ready_queues.pop_front(); 
  }
  DVLOG(1) << "OpenCLQueue executor is finished";
}

bool OpenCLQueueManager::schedule(std::string acc_id, Task* task) {
  if (!task || !kernel_table_.count(acc_id)) {
    throw invalidParam(__func__);
  }
  PLACE_TIMER;

  OpenCLKernelQueue_ptr fastest_q;
  std::string fastest_k;
  uint64_t min_queue_time = (uint64_t)(-1);

  // iterate through all available kernels
  for (auto k : kernel_table_[acc_id]) {
    auto q = queue_table_[k];
    uint64_t wait_time = q->get_wait_time();
    DLOG_IF(INFO, VLOG_IS_ON(2)) << "kernel " << k
      << " has wait time: " << wait_time; 
    if (wait_time < min_queue_time) {
      fastest_q = q;
      fastest_k = k;
      min_queue_time = wait_time;
    } 
  }
  DLOG_IF(INFO, VLOG_IS_ON(2)) << 
    "To schedule a task to kernel: " << fastest_k;
#ifndef NO_PROFILING
  ksight::ksight.add(fastest_k + std::string(" num task"), 1);
#endif

  // schedule to fastest_q
  return fastest_q->enqueue(task);
}

// setup kernels for a given acc
void OpenCLQueueManager::setup_kernels(std::string acc_id) 
{
  if (!kernel_table_.count(acc_id)) {
    LOG_IF(ERROR, VLOG_IS_ON(1)) << "Cannot find acc: " << acc_id;
    throw invalidParam(__func__);
  }
  for (auto kernel_name : kernel_table_[acc_id]) {
    DVLOG(1) << "Setup kernel " << kernel_name;

    ConfigTable_ptr task_conf = conf_table_[kernel_name];
    std::string num_str;
    int         num_sub_kernels;
    if (task_conf->get_conf("num_sub_kernels", num_str)) {
      num_sub_kernels = stoi(num_str);
    }
    else {
      num_sub_kernels = 0;
    }

    // create a new OpenCLEnv with new kernel
    OpenCLEnv* env = dynamic_cast<OpenCLEnv*>(platform->getEnv().lock().get());
    if (!env) {
      DLOG(ERROR) << "invalid OpenCLEnv for kernel: " << kernel_name;
      continue;
    }

    std::vector<cl_kernel> sub_kernels_list;
    if (num_sub_kernels == 0) {
      // allocate OpenCL kernel
      cl_int err = 0;
      cl_program program = env->getProgram();
      cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), &err);
      if (!kernel || err != CL_SUCCESS) {
        std::stringstream ss;
        ss << "Cannot create kernel: " << kernel_name << ": " << err;
        throw internalError(ss.str());
      }
      sub_kernels_list.push_back(kernel);
    }
    else {
      cl_program program = env->getProgram();
      for (int sub_kernel_idx = 0; sub_kernel_idx < num_sub_kernels; sub_kernel_idx++) {
        std::string sub_kname_key = "sub_kernel_name[" + std::to_string(sub_kernel_idx) + "]";
        std::string sub_kname = task_conf->get_conf<std::string>(sub_kname_key);

        cl_int err = 0;
        cl_kernel sub_kernel = clCreateKernel(program, sub_kname.c_str(), &err);
        if (!sub_kernel || err != CL_SUCCESS) {
          std::stringstream ss;
          ss << "Cannot create sub-kernel: " << sub_kname << ": " << err;
          throw internalError(ss.str());
        }
        sub_kernels_list.push_back(sub_kernel);
      }
    }

    // copy env to task_env
    TaskEnv_ptr task_env(new OpenCLEnv(*env));

    ((OpenCLEnv*)task_env.get())->kernels_ = sub_kernels_list;

    env_table_[kernel_name] = task_env;

    // create a kernel queue
    OpenCLKernelQueue_ptr q(new OpenCLKernelQueue(
          task_env, queue_table[acc_id].get(), conf_table_[kernel_name]));

    queue_table_[kernel_name] = q;

    // TODO: also need to handle all the configurations may passed on to
    // kernels
  }
}

void OpenCLQueueManager::remove_kernels(std::string acc_id)
{
  if (!kernel_table_.count(acc_id)) {
    LOG_IF(ERROR, VLOG_IS_ON(1)) << "Cannot find acc: " << acc_id;
    throw invalidParam(__func__);
  }
  for (auto k : kernel_table_[acc_id]) {
    // remove OpenCLKernelQueue from queue_table_
    queue_table_.erase(k);

    // remove kernel from OpenCLEnv
    OpenCLEnv* env = dynamic_cast<OpenCLEnv*>(env_table_[k].get());
    if (!env) {
      DLOG(ERROR) << "invalid env variable";
      throw internalError(__func__);
    }

    // release kernel
    for (int i = 0; i < env->getNumKernels(); i++)
      clReleaseKernel(env->getKernel(i));

    // remove TaskEnv_ptr from env_table_
    //env_table_.erase(k);
  }
}

} // namespace blaze
