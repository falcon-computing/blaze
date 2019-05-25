#ifndef TASK_H
#define TASK_H
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <utility>

#include "blaze/ConfigTable.h"
#include "blaze/Common.h"

namespace blaze {

/**
 * Task is the base clase of an accelerator task
 * will be extended by user 
 */
class Task 
: public boost::basic_lockable_adapter<boost::mutex>
{

friend class AccAgent;
friend class AppCommManager;
friend class QueueManager;
friend class TaskManager;

public:
  Task(int _num_args);
  virtual ~Task();

  // enum type for task status
  enum STATUS {
    NOTREADY,
    READY,
    EXECUTING,
    FINISHED,
    FAILED,
    TIMEOUT,
    COMMITTED
  };

  // main function to be overwritten by accelerator implementations
  virtual void compute() {;}
  virtual uint64_t estimateClientTime() { return 0;}
  virtual uint64_t estimateTaskTime() { return 0; }

  // called before task is executed, usually need to wait for TaskEnv 
  // being set correctly
  virtual void prepare() {;}

  // wrapper around compute(), added indicator for task status
  void execute();

  // check status
  STATUS get_status();

  // wait for task status to update
  void wait();
  
  // get config for input blocks
  // TODO: need a way to specify general configs
  // or config for output block
  std::string getConfig(int idx, std::string key);

  bool isInputReady(int64_t block_id);

  template<typename T> 
  bool get_conf(std::string key, T &val) {
    return conf_->get_conf(key, val);
  }

protected:

  void* getOutput(int idx, 
      int item_length, int num_items, int data_width, 
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  void setOutput(int idx, DataBlock_ptr block);
  
  int getInputLength(int idx);

  int getInputNumItems(int idx);

  void* getInput(int idx);

  // add a configuration for a dedicated block 
  void addConfig(int idx, std::string key, std::string val);

  TaskEnv* getEnv();

  // a list of input blocks to its partition_id
  std::vector<int64_t> input_blocks;

  // list of output blocks
  std::vector<DataBlock_ptr> output_blocks;

  // a table that maps block index to configurations
  std::map<int, std::map<std::string, std::string> > config_table;

  // a mapping between partition_id to the input blocks
  std::map<int64_t, DataBlock_ptr> input_table;

private:

  // used by AppCommManager
  void addInputBlock(int64_t partition_id, DataBlock_ptr block);
  void inputBlockReady(int64_t partition_id, DataBlock_ptr block);

  // push one output block to consumer
  // return true if there are more blocks to output
  bool getOutputBlock(DataBlock_ptr &block);
   
  bool isReady();

  void dumpInput();

  void set_status(STATUS s);

  STATUS status_;

  // an unique id within each TaskQueue
  int task_id;

  // pointer to the TaskEnv
  TaskEnv_ref env;

  // pointer to a config table
  ConfigTable_ptr conf_;

  // number of total input blocks
  int num_input;

  // number of input blocks that has data initialized
  int num_ready;

  // manage multi-thread execution of the task
  boost::condition_variable cv_;
  boost::mutex mtx_;

  // condition variable to check timeout
  const int timeout_seconds_ = 60;
};
}
#endif
