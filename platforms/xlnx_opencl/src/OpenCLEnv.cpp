#include "blaze/xlnx_opencl/OpenCLBlock.h"
#include "blaze/xlnx_opencl/OpenCLEnv.h" 

namespace blaze {
  
DataBlock_ptr OpenCLEnv::create_block(
      int num_items, int item_length,
      int item_size, int align_width, 
      DataBlock::Flag flag,
      ConfigTable_ptr conf)
{
  DataBlock_ptr block(new OpenCLBlock(this,
        num_items, item_length, item_size, 
        align_width, flag, conf));

  return block;
}

DataBlock_ptr OpenCLEnv::create_block(
      std::string path,
      int num_items, int item_length,
      int item_size, 
      int align_width,
      ConfigTable_ptr conf)
{
  DataBlock_ptr block(new OpenCLBlock(this,
        path, num_items, item_length, item_size,
        align_width, conf));

  return block;
}

void OpenCLEnv::changeKernel(cl_kernel& _kernel) {
  kernel = _kernel;
}

void OpenCLEnv::changeProgram(cl_program& _program) {
  program = _program;
}
}
