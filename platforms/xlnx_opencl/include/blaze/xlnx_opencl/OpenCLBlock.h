#ifndef OPENCLBLOCK_H
#define OPENCLBLOCK_H

#include <CL/opencl.h>
#include "blaze/Block.h"
#include "OpenCLCommon.h"

namespace blaze {

class OpenCLBlock : public DataBlock {

public:
  // create a single output elements
  OpenCLBlock(OpenCLEnv* _env, 
      std::string path,
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width = 0,
      DataBlock::Flag _flag = DataBlock::OWNED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

   OpenCLBlock(OpenCLEnv* _env, 
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width = 0,
      DataBlock::Flag _flag = DataBlock::SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  virtual ~OpenCLBlock();

  virtual void alloc();

  // read/write data from/to shared memory
  virtual void readFromMem(std::string path);
  virtual void writeToMem(std::string path);

  // copy data from an array
  virtual void writeData(void* src, size_t _size);
  virtual void writeData(void* src, size_t _size, size_t offset);

  // write data to an array
  virtual void readData(void* dst, size_t size);

  // sample the items in the block by a mask
  virtual DataBlock_ptr sample(char* mask);

  virtual void* getData() { 

    alloc();

    // this is a reinterpretive cast from cl_mem* to char*
    return (void*)&buffer_; 
  }

private:
  OpenCLEnv *env;

  bool   is_allocated_;
  cl_mem buffer_;
  void*  host_ptr_;
};
}

#endif
