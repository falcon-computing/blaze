#ifndef OPENCL_QUEUE_MANAGER_H
#define OPENCL_QUEUE_MANAGER_H

#include "blaze/QueueManager.h"
#include "OpenCLCommon.h"

namespace blaze {

class OpenCLQueueManager : public QueueManager {
public:

  OpenCLQueueManager(
      Platform* _platform,
      int _reconfig_timer = 500);

  virtual ~OpenCLQueueManager();

  // call base class' method
  virtual void add(AccWorker &conf);

  // overriding parent virtual function
  void remove(std::string id);

  void start();

private:
  // thread body of executing tasks from children TaskManagers
  void do_start();

  // schedule a task to one of the kernel queues
  bool schedule(std::string acc_id, Task* task);

  // setup kernel queues and envs for an acc
  void setup_kernels(std::string acc_id);

  // clear kernel queues and envs for a acc
  // this happens when we change cl_program
  // NOTE: this is under the assumption we cannot
  // retain cl_kernel after a cl_program is released
  void remove_kernels(std::string acc_id);

  bool            power_;
  int             reconfig_timer;
  OpenCLPlatform* ocl_platform;

  boost::thread_group executors;

  std::string curr_acc_;

  // map acc_id to conf tables
  //std::map<std::string, AccWorker> acc_conf_table_;

  // map kernel_name to OpenCLEnv
  std::map<std::string, TaskEnv_ptr> env_table_;

  // map acc_id to a list of kernel names
  std::map<std::string, std::vector<std::string> > kernel_table_;

  // map kernel_name to kernel queues
  std::map<std::string, ConfigTable_ptr> conf_table_;

  // map kernel_name to kernel queues
  std::map<std::string, OpenCLKernelQueue_ptr> queue_table_;
};
} // namespace blaze

#endif
