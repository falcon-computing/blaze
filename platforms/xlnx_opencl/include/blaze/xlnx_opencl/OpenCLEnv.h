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

public:
  OpenCLEnv(
      cl_context _context,
      cl_command_queue _queue,
      cl_device_id _device_id): 
    context(_context), 
    cmd_queue(_queue),
    device_id(_device_id)
  {;}

  cl_device_id& getDeviceId() { return device_id; }
  cl_context& getContext() { return context; }
  cl_command_queue& getCmdQueue() { return cmd_queue; }
  cl_kernel& getKernel() { return kernel; }
  cl_program& getProgram() { return program; }

  virtual DataBlock_ptr create_block(
      int num_items, int item_length, int item_size, 
      int align_width = 0, 
      DataBlock::Flag flag = DataBlock::SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  virtual DataBlock_ptr create_block(std::string path,
      int num_items, int item_length, int item_size, 
      int align_width = 0,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

private:
  void changeKernel(cl_kernel& _kernel);
  void changeProgram(cl_program& _program);

  cl_device_id     device_id;
  cl_context       context;
  cl_command_queue cmd_queue;
  cl_kernel        kernel;
  cl_program       program;
};
}
#endif
