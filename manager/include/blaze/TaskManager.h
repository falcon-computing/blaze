#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/atomic.hpp>
#include <boost/memory_order.hpp>

#include "Common.h"
#include "TaskQueue.h"

namespace blaze {

/**
 * Manages a task queue for one accelerator executor
 */
class TaskManager 
: public boost::basic_lockable_adapter<boost::mutex>
{

public:

  TaskManager(
    Task* (*create_func)(), 
    void (*destroy_func)(Task*),
    std::string _acc_id,
    Platform *_platform
  ): power(true),
     nextTaskId(0),
     queue_delay(0),
     acc_id(_acc_id),
     createTask(create_func),
     destroyTask(destroy_func),
     platform(_platform)
  {;}

  virtual ~TaskManager();

  // create a task and return the task pointer
  Task_ptr create();

  // enqueue a task in the corresponding application queue
  void enqueue(std::string app_id, Task* task);

  // dequeue a task from the execute queue
  bool popReady(Task* &task);

  // get best and worst cast wait time 
  std::pair<int, int> getWaitTime(Task* task);

  uint64_t get_queue_delay();
  void modify_queue_delay(uint64_t cur_delay, bool add_or_sub);

  void startExecutor();
  void startScheduler();

  void start();
  void stop();

  bool isEmpty();

  // experimental
  std::string getConfig(int idx, std::string key);

  // set TaskEnv
  void set_env(Task* task, TaskEnv_ptr env);

  // set ConfigTable for task
  void set_conf(Task* task, ConfigTable_ptr conf);

private:
  // Enable signal all the worker threads (scheduler, executor)
  bool power;

  // Locate TaskEnv and for logging purpose
  std::string acc_id;

  Platform *platform;

  TaskQueue execution_queue;
  boost::thread_group task_workers;

  // These two flag let TaskManager exits gracefully:
  // When power=false, but the app_queues and execution_queue
  // are still not empty, clear the queue before exits
  boost::mutex scheduler_lock_;
  boost::mutex executor_lock_;

  mutable boost::atomic<int> nextTaskId;

  // estimated time for all tasks in current queue (in ns)
  mutable boost::atomic<uint64_t> queue_delay;

  // application queues mapped by application id
  std::map<std::string, TaskQueue_ptr> app_queues;

  // Task implementation loaded from user acc_impl
  Task* (*createTask)();
  void (*destroyTask)(Task*);

  // schedule a task from app queues to execution queue  
  bool schedule();

  // execute front task in the queue
  bool execute();

  // thread function body for scheduler and executor
  void do_schedule();
  void do_execute();

  void updateDelayModel(Task* task, int estimateTime, int realTime);
};

const TaskManager_ptr NULL_TASK_MANAGER;
}

#endif
