#include "blaze/altr_opencl/OpenCLBlock.h"
#include "blaze/altr_opencl/OpenCLEnv.h" 

namespace blaze {
  
OpenCLEnv::OpenCLEnv(cl_context context,
      cl_command_queue queue,
      cl_device_id device_id
    ): context_(context), cmd_queue_(queue),
       device_id_(device_id), program_(NULL) 
{
  ;
}

cl_device_id& OpenCLEnv::getDeviceId() { 
  return device_id_;
}

cl_context& OpenCLEnv::getContext() { 
  return context_;
}

cl_command_queue& OpenCLEnv::getCmdQueue() { 
  return cmd_queue_;
}

cl_program& OpenCLEnv::getProgram() {
  return program_;
}

cl_kernel& OpenCLEnv::getKernel() { 
  if (kernels_.empty()) {
    throw invalidParam("Kernel has not initialized");
  }
  return kernels_.begin()->second;
}

cl_kernel& OpenCLEnv::getKernel(std::string name) { 
  if (kernels_.count(name)) {
    return kernels_[name];
  }
  else {
    throw invalidParam("Kernel has not initialized");
  }
}

DataBlock_ptr OpenCLEnv::createBlock(
      int num_items, int item_length,
      int item_size, int align_width, 
      int flag) 
{
  DataBlock_ptr block(new OpenCLBlock(this,
        num_items, item_length, item_size, 
        align_width, flag));

  return block;
}

void OpenCLEnv::addKernel(std::string name, cl_kernel& kernel) {
  kernels_[name] = kernel;
}

void OpenCLEnv::releaseProgram() {

  uint64_t start_t = getUs();
  // First release all the kernels related to this program
  std::map<std::string, cl_kernel>::iterator iter;
  for (iter = kernels_.begin(); iter != kernels_.end(); iter++) {
    clReleaseKernel(iter->second);
  } 
  kernels_.clear();

  // Then release the old cl_program
  if (program_) {
    clReleaseProgram(program_);
  }
  DLOG(INFO) << "Releasing program and kernel takes "
             << getUs() - start_t << " us.";
}

void OpenCLEnv::setProgram(cl_program& program) {
  program_ = program;
}
}
