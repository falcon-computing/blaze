#ifndef OPENCLENV_H
#define OPENCLENV_H

#include <boost/any.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <CL/opencl.h>
#include <map>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>

#include "blaze/Block.h"
#include "blaze/TaskEnv.h"
#include "OpenCLBlock.h"
#include "OpenCLCommon.h"

namespace blaze {

namespace lf = boost::lockfree;

class OpenCLEnv : 
  public TaskEnv, 
  public boost::basic_lockable_adapter<boost::mutex>
{
  friend class OpenCLPlatform;
  friend class OpenCLQueueManager;

public:
  OpenCLEnv(cl_context       _context,
            cl_command_queue _queue,
            cl_device_id     _device_id):
    context_(_context), 
    cmd_queue_(_queue),
    device_id_(_device_id)
  {;}

  OpenCLEnv(const OpenCLEnv &env): TaskEnv(env) {
    // copy everything except kernel_name and kernel 
    device_id_ = env.device_id_; 
    context_   = env.context_;
    cmd_queue_ = env.cmd_queue_;
    program_   = env.program_;
  }

  cl_device_id&     getDeviceId() { return device_id_; }
  cl_context&       getContext() { return context_; }
  cl_command_queue& getCmdQueue() { return cmd_queue_; }
  cl_kernel&        getKernel() { return kernel_; }
  cl_program&       getProgram() { return program_; }
  std::string       get_kernel_name() { return kernel_name_;}

  virtual DataBlock_ptr create_block(
      int num_items, int item_length, int item_size, 
      int align_width = 0, 
      DataBlock::Flag flag = DataBlock::SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr)
  {
    DataBlock_ptr block(new OpenCLBlock(this,
          num_items, item_length, item_size, 
          align_width, flag, conf));

    return block;
  }

  virtual DataBlock_ptr create_block(std::string path,
      int num_items, int item_length, int item_size, 
      int align_width = 0,
      int port = 0,
      DataBlock::Flag flag = DataBlock::OWNED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr)
  {
    DataBlock_ptr block(new OpenCLBlock(this,
          path, num_items, item_length, item_size,
          align_width, port, flag, conf));

    return block;
  }

  // calls to manipulate task_scratch_table_
  // also need to lock
  template <typename T>
  bool putScratch(std::string key, T & value) {
    boost::lock_guard<OpenCLEnv> guard(*this);
    boost::any v = value;
    task_scratch_table_[key].push(v);

    return true;
  }

  template <typename T>
  bool getScratch(std::string key, T & value) {
    boost::lock_guard<OpenCLEnv> guard(*this);

    if (!task_scratch_table_.count(key)) {
      task_scratch_table_[key]; // should construct a queue
      return false;
    }
    else {
      if (task_scratch_table_[key].empty()) {
        return false;
      } 
      else {
        boost::any v = task_scratch_table_[key].front();
        task_scratch_table_[key].pop();
        try {
          value = boost::any_cast<T>(v);
        } catch (boost::bad_any_cast &e) {
          throw invalidParam(__func__);
        }
        return true;
      }
    }
  }

private:
  cl_device_id     device_id_;
  cl_context       context_;
  cl_command_queue cmd_queue_;
  cl_program       program_;
  cl_kernel        kernel_;
  std::string      kernel_name_;

  // a scratch buffer to allow tasks to share some 
  // data in a cyclic queue fashion
  // the Task is total control of the contents,
  // including allocate/free
  // TODO: let blaze handle the management to avoid
  // memory leaks or for protection
  std::map<std::string, std::queue<boost::any> > task_scratch_table_;
};
}
#endif
