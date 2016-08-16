#include <boost/thread/thread.hpp>
#include <glog/logging.h>

#include "Task.h"
#include "TaskManager.h"
#include "CaffePlatform.h"
#include "CaffeQueueManager.h"

namespace blaze 
{
CaffeQueueManager::CaffeQueueManager(
    Platform* platform,
    int reconfig_timer):
  QueueManager(platform),
  reconfig_timer_(reconfig_timer)
{
  caffe_platform_ = dynamic_cast<CaffePlatform*>(platform);

  if (!caffe_platform_) {
    throw std::runtime_error("Cannot create CaffeQueueManager");
  }
  // start executor
  boost::thread executor(
      boost::bind(&CaffeQueueManager::do_start, this));
}

void CaffeQueueManager::start() {
  // do nothing since the executors are already started
  DLOG(INFO) << "CaffeQueue started";
}

void CaffeQueueManager::do_start() {
  VLOG(1) << "Start a executor for CaffeQueueManager";
//  caffe::Caffe::set_mode(caffe::Caffe::GPU);
  caffe::Caffe::set_mode(caffe::Caffe::CPU);

  std::list<std::pair<std::string, TaskManager_ptr> > ready_queues;

  // TODO: need to switch if the accelerator defines other devices
//  caffe::Caffe::set_mode(caffe::Caffe::GPU);
  caffe::Caffe::set_mode(caffe::Caffe::CPU);

  while (1) {
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
        std::map<std::string, TaskManager_ptr>::iterator iter;
        for (iter = queue_table.begin();
            iter != queue_table.end();
            ++iter)
        {
          if (!iter->second->isEmpty()) {
            ready_queues.push_back(*iter);
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
      caffe_platform_->changeNetwork(queue_name);
    }
    catch (std::runtime_error &e) {

      caffe_platform_->removeQueue(queue_name);

      // remove queue_name from ready queue since it's already removed
      ready_queues.pop_front();

      // if setup program keeps failing, remove accelerator from queue_table 
      LOG(ERROR) << "Failed to setup network for " << queue_name
        << ": " << e.what()
        << ". Remove it from CaffeQueueManager.";

      continue;
    }

    // timer to wait for the queue to fill up again
    int counter = 0;
    while (counter < reconfig_timer_) {

      Task* task;
      if (queue->popReady(task)) {
        
        VLOG(1) << "Execute one task from " << queue_name;

        DLOG(INFO) << "Caffe mode is setup as " << caffe::Caffe::mode();

        // execute one task
        try {
          uint64_t start_time = getUs();

          // start execution
          task->execute();

		  std::cout << caffe_platform_->getNet() << std::endl;
		  std::cout << caffe_platform_->getNet()->input_blobs()[0]->channels() << std::endl;
		  std::cout << caffe_platform_->getNet()->input_blobs()[0]->width()<< std::endl;
		  std::cout << caffe_platform_->getNet()->input_blobs()[0]->height()<< std::endl;

          // record task execution time
          uint64_t delay_time = getUs() - start_time;

          VLOG(1) << "Task finishes in " << delay_time << " us";
        } 
        catch (std::runtime_error &e) {
          LOG(ERROR) << "Task error " << e.what();
        }
        // reset the counter
        counter = 0;
      }
      else { 
        DLOG_EVERY_N(INFO, 50) << "Queue " << queue_name 
                               << " empty for " << counter << "ms";

        // start counter
        boost::this_thread::sleep_for(boost::chrono::milliseconds(1)); 
        
        counter++;
      }
    }
    // if the timer is up, switch to the next queue
    ready_queues.pop_front(); 
  }
}
} // namespace blaze
