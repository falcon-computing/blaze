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
  OpenCLEnv(cl_context  context,
      cl_command_queue  queue,
      cl_device_id      device_id);

  cl_device_id& getDeviceId();
  cl_context& getContext();
  cl_command_queue& getCmdQueue();
  cl_program& getProgram();
  cl_kernel& getKernel();
  cl_kernel& getKernel(std::string name);

  virtual DataBlock_ptr createBlock(
      int num_items, 
      int item_length,
      int item_size, 
      int align_width = 0, 
      int flag = BLAZE_OUTPUT_BLOCK);

 private:
  void addKernel(std::string name, cl_kernel& kernel);
  void releaseProgram();
  void setProgram(cl_program& program);

  cl_device_id     device_id_;
  cl_context       context_;
  cl_command_queue cmd_queue_;
  cl_program       program_;
  std::map<std::string, cl_kernel> kernels_;
};
}
#endif
