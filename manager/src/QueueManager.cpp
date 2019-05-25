#include <boost/filesystem.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <dlfcn.h>
#include <fstream>
#include <stdexcept>

#ifdef NDEBUG
#define LOG_HEADER "QueueManager"
#endif
#include <glog/logging.h>

#include "blaze/Task.h"
#include "blaze/Block.h"
#include "blaze/Platform.h"
#include "blaze/TaskManager.h"
#include "blaze/QueueManager.h"

namespace blaze {

QueueManager::~QueueManager() {
  DVLOG(1) << "QueueManager is destroyed";
}

void QueueManager::add(AccWorker &conf) {
  
  std::string id = conf.id();
  std::string lib_path = conf.path();

  if (tasklib_table.count(id)) {
    LOG_IF(WARNING, VLOG_IS_ON(1)) << "Cannot add Task [" << id
                 << "] because previous Task with the same ID is not "
                 << "successfully unloaded.";
    throw internalError("Task handle exists");
  }
  void* handle = dlopen(lib_path.c_str(), RTLD_LAZY|RTLD_LOCAL);

  if (handle == NULL) {
    throw std::runtime_error(dlerror());
  }
  // reset errors
  dlerror();

  // load the symbols
  Task* (*create_func)();
  void (*destroy_func)(Task*);

  // read the custom constructor and destructor  
  create_func = (Task* (*)())dlsym(handle, "create");
  destroy_func = (void (*)(Task*))dlsym(handle, "destroy");

  const char* error = dlerror();
  if (error) {
    throw std::runtime_error(error);
  }

  // construct the corresponding task queue
  TaskManager_ptr task_manager(
      new TaskManager(create_func, destroy_func, id, platform));

  task_manager->startScheduler();

  // lock before modifying queue_table
  boost::lock_guard<QueueManager> guard(*this);

  queue_table.insert(std::make_pair(id, task_manager));
  tasklib_table.insert(std::make_pair(id, handle));

  VLOG(1) << "Added a new task queue: " << id;
}

TaskManager_ptr QueueManager::get(std::string id) {

  boost::lock_guard<QueueManager> guard(*this);
  if (queue_table.find(id) == queue_table.end()) {
    return NULL_TASK_MANAGER;   
  }
  else {
    return queue_table[id];
  }
}

void QueueManager::remove(std::string id) {

  TaskManager_ptr task_manager = get(id);

  if (!task_manager) {
    return;
  }

  this->lock();
  queue_table.erase(id);
  this->unlock();

  DVLOG(2) << "Stopping the TaskManager for " << id;
  task_manager->stop();

  // wait for TaskManager to exit gracefully
  //while (task_manager->isBusy()) {
  //  boost::this_thread::sleep_for(boost::chrono::microseconds(1000));
  //}
  DVLOG(1) << "TaskManager for " << id << " is successfully stopped";

  // release the loaded library using dlclose
  // reset errors
  dlerror();

#if 0 // don't unload task
  void* handle = tasklib_table[id];

  dlclose(handle);

  const char* error = dlerror();
  if (error) {
    DLOG(ERROR) << "Task implementation for " << id 
                << " cannot be unloaded because:" 
                << error;
  }
  else {
    DVLOG(2) << "Task implementation for " << id << " is successfully unloaded";

    // only remove item from table if dlclose is successful
    // otherwise prevent new accelerator with the same id 
    // be added
    this->lock();
    tasklib_table.erase(id);
    this->unlock();
  }
#endif
}

TaskEnv* QueueManager::getTaskEnv(Task* task) {
  return task->getEnv();
}

void QueueManager::setTaskEnv(Task* task, TaskEnv_ptr env) {
  task->env = env;
}

DataBlock_ptr QueueManager::getTaskInputBlock(Task *task, int idx) {
  if (idx < task->input_blocks.size() &&
      task->input_table.find(task->input_blocks[idx]) 
        != task->input_table.end())
  {
    return task->input_table[task->input_blocks[idx]];
  } else {
    return NULL_DATA_BLOCK; 
  }
}

void QueueManager::setTaskInputBlock(
    Task *task, 
    DataBlock_ptr block, 
    int idx) 
{
  if (idx < task->input_blocks.size()) {
    int64_t block_id = task->input_blocks[idx];
    DVLOG(2) << "Reset task input block " << block_id;
    task->inputBlockReady(block_id, block);
  }
}

// Start TaskQueues for the CPU platform
// all the task queues can have simultaneous executors
void QueueManager::start(std::string id) {

  // get the reference to the task queue 
  TaskManager_ptr task_manager = get(id);
  if (task_manager) {
    task_manager->startExecutor();
  }
  else {
    DLOG(ERROR) << "Cannot start executor for " << id;
  }
}

} // namespace blaze

