#ifndef CAFFEQUEUEMANAGER_H
#define CAFFEQUEUEMANAGER_H

#include "QueueManager.h"

namespace blaze 
{
class CaffeQueueManager : public QueueManager {
  public:
    CaffeQueueManager(Platform* platform,
      int reconfig_timer = 500);

    void start();

  private: 
    CaffePlatform* caffe_platform_;
    int reconfig_timer_;

    // thread body of executing tasks from children TaskManagers
    void do_start();
};
} // namespace blaze
#endif
