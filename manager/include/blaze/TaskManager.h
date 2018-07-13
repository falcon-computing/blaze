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
typedef boost::shared_ptr<boost::thread> Thread_ptr;
class TaskManager 
: public boost::basic_lockable_adapter<boost::mutex>
{

public:
  TaskManager(Task* (*create_func)(), 
    void (*destroy_func)(Task*),
    std::string _acc_id,
    Platform *_platform);

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

  // interrupt Executor in case there are some issues
  void interruptExecutor();

  void startExecutor();
  void startScheduler();

  void start();
  void stop();
  bool isBusy();

  bool isEmpty();

  // experimental
  std::string getConfig(int idx, std::string key);

private:

  // schedule a task from app queues to execution queue  
  bool schedule();

  // execute front task in the queue
  bool execute();

  // thread function body for scheduler and executor
  void do_schedule();
  void do_execute();

  void updateDelayModel(Task* task, int estimateTime, int realTime);

  // scheduler thread and executor thread
  Thread_ptr executor_thread_;
  Thread_ptr scheduler_thread_;

  // Enable signal all the worker threads (scheduler, executor)
  bool power;

  // These two flag let TaskManager exits gracefully:
  // When power=false, but the app_queues and execution_queue
  // are still not empty, clear the queue before exits
  bool scheduler_idle;
  bool executor_idle;

  // Locate TaskEnv and for logging purpose
  std::string acc_id;

  mutable boost::atomic<int> nextTaskId;

  //estimated time for the current queue to finished, units: nanosecs
  mutable boost::atomic<uint64_t> queue_delay;

  // Task implementation loaded from user acc_impl
  Task* (*createTask)();
  void (*destroyTask)(Task*);

  Platform *platform;

  // application queues mapped by application id
  std::map<std::string, TaskQueue_ptr> app_queues;

  TaskQueue execution_queue;
};

const TaskManager_ptr NULL_TASK_MANAGER;
}

#endif
