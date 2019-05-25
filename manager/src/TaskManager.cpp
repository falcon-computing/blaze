#include <boost/atomic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef NDEBUG
#define LOG_HEADER "TaskManager"
#endif
#include <glog/logging.h>

#include "blaze/TaskEnv.h"
#include "blaze/Task.h"
#include "blaze/Block.h"
#include "blaze/TaskQueue.h"
#include "blaze/TaskManager.h"
#include "blaze/Platform.h"

namespace blaze {

TaskManager::~TaskManager() {
  power = false; 
  task_workers.interrupt_all();
  task_workers.join_all();
  DVLOG(2) << "TaskManager is destroyed";
}

bool TaskManager::isEmpty() {
  return execution_queue.empty();
}

void TaskManager::set_env(Task* task, TaskEnv_ptr env) {
  if (task) task->env = env;
}

void TaskManager::set_conf(Task* task, ConfigTable_ptr conf) {
  if (task) task->conf_ = conf;
}

Task_ptr TaskManager::create() {
  
  // create a new task by the constructor loaded form user implementation
  Task_ptr task(createTask(), destroyTask);

  // link the TaskEnv
  task->env = platform->getEnv();

  // give task an unique ID
  task->task_id = nextTaskId.fetch_add(1);

  return task;
}

uint64_t TaskManager::get_queue_delay(){
  return queue_delay.load();
}

void TaskManager::modify_queue_delay(uint64_t cur_delay, bool add_or_sub) {
  if (add_or_sub) {
    uint64_t before = get_queue_delay();
    queue_delay.fetch_add(cur_delay);
    DVLOG(2) << "Queueing delay increases from " << before
      << " to " << get_queue_delay();
  }
  else {
    uint64_t before = get_queue_delay();
    queue_delay.fetch_sub(cur_delay);
    DVLOG(2) << "Queueing delay decreases from " << before
      << " to " << get_queue_delay();
  }
}

void TaskManager::enqueue(std::string app_id, Task* task) {

  if (!task->isReady()) {
    throw std::runtime_error("Cannot enqueue task that is not ready");
  }
  
  // TODO: when do we remove the queue?
  // create a new app queue if it does not exist

  TaskQueue_ptr queue;
  // TODO: remove this lock
  this->lock();
  if (app_queues.find(app_id) == app_queues.end()) {
    TaskQueue_ptr new_queue(new TaskQueue());
    app_queues.insert(std::make_pair(app_id, new_queue));
    queue = new_queue;
  } 
  else {
    queue = app_queues[app_id];
  }
  this->unlock();

  // push task to queue
  bool enqueued = queue->push(task);
  while (!enqueued) {
    boost::this_thread::sleep_for(boost::chrono::microseconds(0)); 
    enqueued = queue->push(task);
  }
}

bool TaskManager::schedule() {
  boost::mutex::scoped_lock lock(scheduler_lock_);

  if (app_queues.empty()) {
    return false;
  }
  // iterate through all app queues and record which are non-empty
  std::vector<std::string> ready_queues;
  std::map<std::string, TaskQueue_ptr>::iterator iter;

  this->lock();
  for (iter = app_queues.begin();
      iter != app_queues.end();
      iter ++)
  {
    if (iter->second && !iter->second->empty()) {
      ready_queues.push_back(iter->first);
    }
  }
  this->unlock();
  if (ready_queues.empty()) {
    return false;
  }

  Task* next_task;

  // select the next task to execute from application queues
  // use RoundRobin scheduling
  int idx_next = rand()%ready_queues.size();

  if (app_queues.find(ready_queues[idx_next]) == app_queues.end()) {
    DLOG(ERROR) << "Did not find app_queue, unexpected";
    return true;
  }
  else {
    app_queues[ready_queues[idx_next]]->pop(next_task);
  }
  if (next_task) {
    execution_queue.push(next_task);

    VLOG(1) << "Schedule a task to execute from " << ready_queues[idx_next];
  }

  return true;
}

// return true if the execution queue is empty
bool TaskManager::execute() {
  boost::mutex::scoped_lock lock(executor_lock_);

  // wait if there is no task to be executed
  if (execution_queue.empty()) {
    return false;
  }
  // get next task and remove it from the task queue
  // this part is thread-safe with boost::lockfree::queue
  Task* task;
  execution_queue.pop(task);

  if (!task) {
    LOG(ERROR) << "Task already destroyed";
    return false;
  }

  VLOG(2) << "Started a new task";

  // record task execution time
  uint64_t start_time = getUs();

  // start execution
  task->execute();

  return true;
}

bool TaskManager::popReady(Task* &task) {
  if (execution_queue.empty()) {
    return false;
  }
  else {
    execution_queue.pop(task);
    return true;
  }
}

std::string TaskManager::getConfig(int idx, std::string key) {
  Task* task = (Task*)createTask();

  std::string config = task->getConfig(idx, key);

  destroyTask(task);
  
  return config;
}

void TaskManager::do_execute() {

  VLOG(1) << "Started an executor for " << acc_id;

  // continuously execute tasks from the task queue
  while (power) { 
    if (execute()) {
      boost::this_thread::sleep_for(boost::chrono::microseconds(100)); 
    }
  }

  VLOG(1) << "Executor for " << acc_id << " stopped";
}

void TaskManager::do_schedule() {
  
  VLOG(1) << "Started an scheduler for " << acc_id;

  while (power) {
    // return true if the app queues are all empty
    if (schedule()) {
      boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    }
  }

  VLOG(1) << "Scheduler for " << acc_id << " stopped";
}

//bool TaskManager::isBusy() {
//  return scheduler_lock_.try_lock() || executor_lock_.try_lock();
//}

void TaskManager::start() {
  startExecutor();
  startScheduler();
}

void TaskManager::stop() {
  power = false;
  boost::mutex::scoped_lock l1(scheduler_lock_);
  boost::mutex::scoped_lock l2(executor_lock_);
}

void TaskManager::startExecutor() {
  task_workers.create_thread(
      boost::bind(&TaskManager::do_execute, this));
}

void TaskManager::startScheduler() {
  task_workers.create_thread(
      boost::bind(&TaskManager::do_schedule, this));
}

} // namespace blaze
