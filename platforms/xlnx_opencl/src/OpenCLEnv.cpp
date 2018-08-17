#include "blaze/xlnx_opencl/OpenCLBlock.h"
#include "blaze/xlnx_opencl/OpenCLEnv.h" 

namespace blaze {
  
OpenCLEnv::OpenCLEnv(const OpenCLEnv & env): TaskEnv(env) {
  // copy everything except kernel_name and kernel 
  device_id_ = env.device_id_; 
  context_   = env.context_;
  cmd_queue_ = env.cmd_queue_;
  program_   = env.program_;
}

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
} // namespace blaze
