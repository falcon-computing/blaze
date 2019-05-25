#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <stdio.h>

#ifdef NDEBUG
#define LOG_HEADER "Task"
#endif
#include <glog/logging.h>

#ifdef USEHDFS
#include "hdfs.h"
#endif

#include "blaze/Block.h"
#include "blaze/TaskEnv.h"
#include "blaze/Task.h"
#include "blaze/Platform.h"

namespace blaze {

Task::Task(int _num_args):
    status_(NOTREADY), 
    num_input(_num_args),
    num_ready(0)
{
  ; 
}

Task::~Task() {
  DVLOG(2) << "Task is destroyed";
}

TaskEnv* Task::getEnv() { 
  if (env.lock()) return env.lock().get();
  else {
    DLOG(ERROR) << "TaskEnv is already destroyed";
    throw runtimeError(__func__);
  }
}

bool Task::isInputReady(int64_t block_id) {
  if (input_table.find(block_id) != input_table.end() &&
      input_table[block_id]->isReady()) 
  {
    return true;
  } else {
    return false;
  }
}

void* Task::getOutput(
    int idx, 
    int item_length, 
    int num_items,
    int data_width,
    ConfigTable_ptr conf) 
{
  if (idx < output_blocks.size()) {
    // if output already exists, return the pointer 
    // to the existing block
    return output_blocks[idx]->getData();
  }
  else {
    // if output does not exist, create one
    DataBlock_ptr block = env.lock()->create_block(num_items, 
        item_length, item_length*data_width, 0, DataBlock::SHARED, conf);

    output_blocks.push_back(block);

    return block->getData();
  }
}

void Task::setOutput(int idx, DataBlock_ptr block) {
  if (idx >= output_blocks.size()) 
    output_blocks.resize(idx+1);

  output_blocks[idx] = block;
}

int Task::getInputLength(int idx) { 
  if (idx < input_blocks.size() && 
      input_table.find(input_blocks[idx]) != input_table.end())
  {
    return input_table[input_blocks[idx]]->getLength(); 
  }
  else {
    throw std::runtime_error("getInputLength out of bound idx");
  }
}


int Task::getInputNumItems(int idx) { 
  if (idx < input_blocks.size() &&
      input_table.find(input_blocks[idx]) != input_table.end())
  {
    return input_table[input_blocks[idx]]->getNumItems() ; 
  }
  else {
    throw std::runtime_error("getInputNumItems out of bound idx");
  }
}

void* Task::getInput(int idx) {

  if (idx < input_blocks.size() &&
      input_table.find(input_blocks[idx]) != input_table.end())
  {
    return input_table[input_blocks[idx]]->getData();      
  }
  else {
    DLOG(ERROR) << "getInput out of bound idx";
    throw internalError(__func__ + std::string(" failed"));
  }
}

void Task::addConfig(int idx, std::string key, std::string val) {
  config_table[idx][key] = val;
}

std::string Task::getConfig(int idx, std::string key) 
{
  if (config_table.find(idx) != config_table.end() &&
      config_table[idx].find(key) != config_table[idx].end()) 
  {
    return config_table[idx][key];
  } else {
    return std::string();
  }
}

void Task::addInputBlock(
    int64_t partition_id, 
    DataBlock_ptr block = NULL_DATA_BLOCK) 
{
  if (input_blocks.size() >= num_input) {
    DLOG(ERROR) << "Inconsistancy between num_args in ACC Task" <<
        " with the number of blocks in ACCREQUEST";
    throw internalError(__func__ + std::string(" failed"));
  }
  // add the block to the input list
  input_blocks.push_back(partition_id);

  if (block != NULL_DATA_BLOCK) {
    // add the same block to a map table to provide fast access
    input_table.insert(std::make_pair(partition_id, block));

    // automatically trace all the blocks,
    // if all blocks are initialized with data, 
    // set the task status to READY
    if (block->isReady()) {
      num_ready ++;
      if (num_ready == num_input) {
        set_status(READY);
      }
    }
  }
}

void Task::dumpInput() {
  // only dump when input is ready
  if (get_status() != NOTREADY) {
    std::string uid = getUid();
    std::string ts  = getTS();
    for (int i = 0; i < num_input; i++) {
      std::stringstream dump_fname;
      dump_fname << "/tmp/blaze-task" 
                 << "-" << uid
                 << "-" << ts
                 << "-i" << i
                 << ".dat";
      
      std::ofstream fout(dump_fname.str(), std::ios::out | std::ios::binary);
      int64_t par_id = input_blocks[i];
      if (input_table.count(par_id) == 0) {
        // internal error
        DLOG(ERROR) << "block not found";
        return;
      }
      DataBlock_ptr b = input_table[par_id];
      if (b) {
        fout.write((char*)b->getData(), b->getSize());
      }
      fout.close();
    } 
  }
}

void Task::inputBlockReady(int64_t partition_id, DataBlock_ptr block) {

  if (input_table.find(partition_id) == input_table.end()) {

    // add the same block to a map table to provide fast access
    input_table.insert(std::make_pair(partition_id, block));

    // assuming the block is already ready
    if (!block || !block->isReady()) {
      throw std::runtime_error("Task::inputBlockReady(): block not ready");
    }
    num_ready ++;
    if (num_ready == num_input) {
      set_status(READY);
    }
  }
  else {
    // overlay this method to set input block to a new block 
    if (!block || !block->isReady()) {
      throw std::runtime_error("Task::inputBlockReady(): block not ready");
    }

    input_table[partition_id] = block;
  }
}

// push one output block to consumer
// return true if there are more blocks to output
bool Task::getOutputBlock(DataBlock_ptr &block) {

  if (!output_blocks.empty()) {

    block = output_blocks.front();

    // assuming the blocks are controlled by consumer afterwards
    output_blocks.erase(output_blocks.begin(), output_blocks.begin()+1);

    // no more output blocks means all data are consumed
    if (output_blocks.empty()) {
      set_status(COMMITTED);
    }
    return true;
  }
  else {
    return false;
  }
}

// check if all the blocks in task's input list is ready
bool Task::isReady() {

  if (get_status() == READY) {
    return true; 
  }
  else if (input_table.size() < num_input) {
    return false;
  }
  else {
    bool ready = true;
    int num_ready_curr = 0;
    for (auto b : input_table) {
      // a block may be added but not initialized
      if (!b.second || !b.second->isReady()) {
        ready = false;
        break;
      }
      num_ready_curr ++;
    }
    if (ready && num_ready_curr == num_input) {
      DLOG(INFO) << "Setting status to ready";
      set_status(READY);
      DLOG(INFO) << "Set status to ready";
      return true;
    }
    else {
      return false;
    }
  }
}

void Task::set_status(STATUS s) {
  boost::lock_guard<Task> guard(*this);
  status_ = s;
}

Task::STATUS Task::get_status() {
  boost::lock_guard<Task> guard(*this);
  return status_;
}

void Task::wait() {
  boost::unique_lock<boost::mutex> l(mtx_);

  // calculate timeout threshold
  uint64_t task_time = this->estimateTaskTime();
  DLOG(INFO) << "estimated time: " << task_time;
  uint64_t timeout_us = 0;
  if (task_time <= 0) {
    timeout_us = timeout_seconds_ * 1e6;
  }
  else {
    // 16x estimated task time ns to us
    timeout_us = (task_time << 4) / 1000;
  }

  if (cv_.wait_for(l, boost::chrono::microseconds(timeout_us)) == 
      boost::cv_status::timeout) {

    LOG_IF(ERROR, VLOG_IS_ON(1)) << "Task timeout in " << 
      timeout_us << " us";

    set_status(TIMEOUT);
  }
}

void Task::execute() {

  // handling timeout
  set_status(EXECUTING);

  try {
    // record task execution time
    uint64_t start_time = getUs();
  
    compute();

    uint64_t delay_time = getUs() - start_time;
    VLOG(1) << "Task finishes in " << delay_time << " us";

    set_status(FINISHED);
  } catch (std::exception &e) {
    set_status(FAILED);
    LOG_IF(ERROR, VLOG_IS_ON(1)) << "Task failed: " << e.what();
  }  
  // notify that task is done
  cv_.notify_one();
}

} // namespace
