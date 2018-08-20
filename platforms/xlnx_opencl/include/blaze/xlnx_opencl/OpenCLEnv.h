#ifndef OPENCLENV_H
#define OPENCLENV_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <CL/opencl.h>

#include "blaze/TaskEnv.h"
#include "OpenCLCommon.h"

namespace blaze {

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

  OpenCLEnv(const OpenCLEnv &env);

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
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  virtual DataBlock_ptr create_block(std::string path,
      int num_items, int item_length, int item_size, 
      int align_width = 0,
      DataBlock::Flag flag = DataBlock::OWNED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

private:
  cl_device_id     device_id_;
  cl_context       context_;
  cl_command_queue cmd_queue_;
  cl_program       program_;
  cl_kernel        kernel_;
  std::string      kernel_name_;
};
}
#endif
