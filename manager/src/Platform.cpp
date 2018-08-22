#ifdef NDEBUG
#define LOG_HEADER "Platform"
#endif
#include <glog/logging.h>

#include "blaze/TaskEnv.h"
#include "blaze/TaskManager.h"
#include "blaze/BlockManager.h"
#include "blaze/QueueManager.h"
#include "blaze/Platform.h"

namespace blaze {

Platform::Platform(std::map<std::string, std::string> &conf_table)
{
  TaskEnv_ptr env_ptr(new TaskEnv());
  env = env_ptr;

  // create queue
  QueueManager_ptr queue(new QueueManager(this));
  queue_manager = queue;

}
Platform::~Platform() {
  DVLOG(1) << "Platform is removed";
}

// Start TaskQueues for the CPU platform
// all the task queues can have simultaneous executors
void Platform::addQueue(AccWorker &conf) {

  if (acc_table.find(conf.id()) == acc_table.end()) {
    acc_table.insert(std::make_pair(conf.id(), conf));
  }

  // add a TaskManager, and the scheduler should be started
  queue_manager->add(conf);

  // start a corresponding executor
  queue_manager->start(conf.id());
}

void Platform::removeQueue(std::string id) {

  // TODO: we should still block until this is finished, 
  // futher performance impact is pending assessment 
  queue_manager->remove(id);
  //boost::thread executor(
  //    boost::bind(&QueueManager::remove, queue_manager.get(), id));
}

// remove a shard block from the block manager
void Platform::remove(int64_t block_id) {
  block_manager->remove(block_id); 
}

void Platform::createBlockManager(size_t cache_limit, size_t scratch_limit) {
  BlockManager_ptr bman(new BlockManager(this, cache_limit, scratch_limit));
  block_manager = bman;
}

QueueManager* Platform::getQueueManager() {
  if (queue_manager) {
    return queue_manager.get();
  } else {
    return NULL;
  }
}

BlockManager* Platform::getBlockManager() {
  if (block_manager) {
    return block_manager.get();
  } else {
    return NULL;
  }
}

TaskManager_ref Platform::getTaskManager(std::string acc_id) {
  return queue_manager->get(acc_id);
}

// get an entry in the config_table matching the key
std::string Platform::getConfig(std::string &key) {
  if (config_table.find(key)==config_table.end()) {
    return std::string();
  } else {
    return config_table[key];
  }
}

// get TaskEnv to pass to Task
TaskEnv_ref Platform::getEnv() {
  return env;
}
} // namespace blaze
