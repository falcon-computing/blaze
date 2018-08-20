#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "blaze/xlnx_opencl/OpenCLKernelQueue.h"
#include "blaze/Task.h"
#include "blaze/TaskEnv.h"
#include "blaze/TaskManager.h"
#include "blaze/Timer.h"

namespace blaze {

OpenCLKernelQueue::OpenCLKernelQueue(
    TaskEnv_ptr env,
    TaskManager* task_manager,
    ConfigTable_ptr conf):
  task_manager_(task_manager), 
  env_(env), conf_(conf), 
  power_(true), has_time_estimate_(true),
  num_tasks_(0), wait_time_(0)
{
  // start a thread to execute
  boost::shared_ptr<boost::thread> t(new boost::thread(
      boost::bind(&OpenCLKernelQueue::do_execute, this)));

  executor_ = t;
}

OpenCLKernelQueue::~OpenCLKernelQueue() {
  // interrupt thread
  executor_->interrupt();
  executor_->join();
}

bool OpenCLKernelQueue::enqueue(Task* task) {
  if (!task) throw invalidParam(__func__);

  task_manager_->set_env(task, env_);
  task_manager_->set_conf(task, conf_);

  if (queue_.push(task)) {
    // increase wait time and num_task
    num_tasks_.fetch_add(1);

    if (has_time_estimate_) {
      uint64_t est = task->estimateTaskTime();
      if (est == 0) {
        has_time_estimate_ = false;
      }
      else {
        wait_time_.fetch_add(est);
      }
    }

    return true;
  }
  else {
    return false;
  }
}

uint64_t OpenCLKernelQueue::get_num_tasks() {
  return num_tasks_.load();
}

uint64_t OpenCLKernelQueue::get_wait_time() {
  if (has_time_estimate_) return wait_time_.load();
  else return num_tasks_.load();
}

void OpenCLKernelQueue::do_execute() {
  while (power_) {
    Task* task = nullptr;
    while (!queue_.pop(task)) {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
    }

    if (!task) {
      LOG(ERROR) << "Task already destroyed";
      continue;
    }

    // execute one task
    try {
      PLACE_TIMER1("Execute Task");

      // start execution
      task->execute();
    }
    catch (std::runtime_error &e) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "Task error " << e.what();
    }

    // decrease wait time and num_task
    num_tasks_.fetch_sub(1);
    if (has_time_estimate_) {
      wait_time_.fetch_sub(task->estimateTaskTime());
    }
  }
}
} // namespace blaze
