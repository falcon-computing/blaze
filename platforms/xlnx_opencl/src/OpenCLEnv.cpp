#include "blaze/xlnx_opencl/OpenCLBlock.h"
#include "blaze/xlnx_opencl/OpenCLEnv.h" 

namespace blaze {
  
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

DataBlock_ptr OpenCLEnv::createBlock(
      int num_items, int item_length,
      int item_size, 
      std::pair<std::string, int>& ext_flag,
      int align_width, 
      int flag) 
{
  DataBlock_ptr block(new OpenCLBlock(this,
        num_items, item_length, item_size, ext_flag, 
        align_width, flag));

  return block;
}

void OpenCLEnv::changeKernel(cl_kernel& _kernel) {
  kernel = _kernel;
}
}
