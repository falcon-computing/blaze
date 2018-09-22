#ifndef BLAZE_XLNX_OPENCLKERNELQUEUE_H
#define BLAZE_XLNX_OPENCLKERNELQUEUE_H

#include <boost/atomic.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <cstdint>

#include "blaze/ConfigTable.h"
#include "blaze/TaskQueue.h"
#include "OpenCLCommon.h"
#include "OpenCLEnv.h"

namespace blaze {
  
class OpenCLKernelQueue {
 public:
  OpenCLKernelQueue(
      TaskEnv_ptr env,
      TaskManager* task_manager,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  ~OpenCLKernelQueue();

  bool enqueue(Task* task);

  uint64_t get_num_tasks();
  uint64_t get_wait_time();

  // finish all the tasks
  void flush();

 private:
  void do_execute();
  void do_prepare();

  TaskManager* task_manager_;
  TaskEnv_ptr  env_;

  // configurations to pass to task
  ConfigTable_ptr conf_;
  bool power_;
  bool has_time_estimate_;
  std::string kernel_name_;

  // for task estimation
  mutable boost::atomic<uint64_t> num_tasks_;
  mutable boost::atomic<uint64_t> wait_time_;

  TaskQueue prep_queue_;
  TaskQueue exe_queue_;

  boost::shared_ptr<boost::thread> preparer_;
  boost::shared_ptr<boost::thread> executor_;
};
} // namespace blaze
#endif
