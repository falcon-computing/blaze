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

  void start();

private:
  OpenCLPlatform* ocl_platform;
  bool power_;

  // thread body of executing tasks from children TaskManagers
  void do_start();

  int reconfig_timer;
  boost::thread_group executors;
};
} // namespace blaze

#endif
