#include <boost/smart_ptr.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
#define LOG_HEADER  "OpenCLBlock"
#endif
#include <glog/logging.h>

#include "blaze/altr_opencl/OpenCLBlock.h"
#include "blaze/altr_opencl/OpenCLEnv.h"

namespace blaze {

OpenCLBlock::OpenCLBlock(OpenCLEnv* _env, 
      std::string path,
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width,
      DataBlock::Flag _flag,
      ConfigTable_ptr conf):
  env(_env), 
  is_allocated_(false),
  buffer_(nullptr),
  DataBlock(path, _num_items, _item_length, _item_size, _align_width, _flag, conf)
{
  ;
}

OpenCLBlock::OpenCLBlock(OpenCLEnv* _env, 
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width,
      DataBlock::Flag _flag,
      ConfigTable_ptr conf):
  env(_env), 
  is_allocated_(false),
  buffer_(nullptr),
  DataBlock(_num_items, _item_length, _item_size, _align_width, _flag, conf)
{
  ;
}
  
OpenCLBlock::~OpenCLBlock() {
  if (is_allocated_) {
    clReleaseMemObject(buffer_);
  }
  DVLOG(1) << "Destroyed one OpenCLBlock";
}

void OpenCLBlock::alloc() {
  if (is_allocated_) return;
  PLACE_TIMER;

  // NOTE: assuming buffer allocation is thread-safe
  cl_context context = env->getContext();
  cl_int err = 0;

  // TODO: use memory_mapped address as host_ptr for now
  // do we need to align it?

  /*
  int bank_id = 0;
  if (conf_->get_conf("bankID", bank_id)) {
    ext_flag.flags = bankID[bank_id];
    DVLOG(2) << "Setting bankID = " << bank_id;
  }
  ext_flag.obj = mm_region_->get_address();
  ext_flag.param = 0;
  */

  DVLOG(3) << "Allocating OpenCLBlock of size " << 
    (double)size_ /1024/1024 << "MB";

  buffer_ = clCreateBuffer(
      context, 
      CL_MEM_READ_WRITE,
      size_, NULL, &err);

  if (err != CL_SUCCESS) {
    throw std::runtime_error("Failed to allocate OpenCL block");
  }
  is_allocated_ = true;
}

void OpenCLBlock::readFromMem(std::string path) {
  // then write temp buffer to FPGA, will be serialized among all tasks
  writeData(NULL, size_);
}

void OpenCLBlock::writeToMem(std::string path) {
  // first copy data from FPGA to a temp buffer, will be serialized among all tasks
  readData(NULL, size_);
}

void OpenCLBlock::writeData(void* src, size_t _size) {
  PLACE_TIMER;
  if (_size > size_) {
    throw std::runtime_error("Not enough space left in Block");
  }

  // lazy allocation
  alloc();

  uint64_t start_t = getUs();
  writeData(src, _size, 0);
  is_ready_ = true;
}

void OpenCLBlock::writeData(void* src, size_t _size, size_t offset) {
  // NOTE: src is not used
  PLACE_TIMER;
  if (offset+_size > size_) {
    throw std::runtime_error("Exceeds block size");
  }

  // lazy allocation
  alloc();

  // get the command queue handler
  cl_command_queue command = env->getCmdQueue();
  //cl_event event;

  // use a lock on TaskEnv to guarantee single-thread access to command queues
  // NOTE: this is unnecessary if the OpenCL runtime is thread-safe
  //boost::lock_guard<OpenCLEnv> guard(*env);
  //env->lock();
  
  int err = clEnqueueWriteBuffer(command, 
      buffer_, CL_TRUE, offset, _size,
      mm_region_->get_address(), 0, NULL, NULL);

  if (err != CL_SUCCESS) {
    DLOG(ERROR) << "clEnqueueWriteBuffer error: " << err;
    DLOG(ERROR) << "block infomation: size=" << _size ;
    LOG_IF(ERROR, VLOG_IS_ON(1)) << "failed to write host data to cl_buffer";
    throw runtimeError(__func__);
  }

  //env->unlock();

  if (offset + _size == size_) {
    is_ready_ = true;
  } // TODO: need to confirm the logics here
}

// write data to an array
void OpenCLBlock::readData(void* dst, size_t size) {
  // NOTE: dst is not used
  if (is_allocated_) {
    PLACE_TIMER;
    // get the command queue handler
    cl_command_queue command = env->getCmdQueue();
    //cl_event event;

    // use a lock on TaskEnv to guarantee single-thread 
    // access to command queues
    // NOTE: this is unnecessary if the OpenCL runtime is thread-safe
    //boost::lock_guard<OpenCLEnv> guard(*env);

    int err = clEnqueueReadBuffer(command, 
        buffer_, CL_TRUE, 0, size,
        mm_region_->get_address(), 0, NULL, NULL);

    if (err != CL_SUCCESS) {
      DLOG(ERROR) << "clEnqueueReadBuffer error: " << err;
      DLOG(ERROR) << "block infomation: size=" << size;
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "failed to write host data to cl_buffer";
      throw runtimeError(__func__);
    }
  }
  else {
    throw std::runtime_error("Block memory not allocated");
  }
}

DataBlock_ptr OpenCLBlock::sample(char* mask) {
#if 0
  // count the total number of 
  int masked_items = 0;
  for (int i=0; i<num_items; i++) {
    if (mask[i]!=0) {
      masked_items ++;
    }
  }

  OpenCLBlock* ocl_block = new OpenCLBlock(env,
        item_length, 
        item_size,
        aligned ? align_width : item_size);

  DataBlock_ptr block(ocl_block);

  cl_mem masked_data = *((cl_mem*)(ocl_block->getData()));

  // get the command queue handler
  cl_command_queue command = env->getCmdQueue();
  cl_int err = 0;

  // start copy the masked data items to the new block,
  // since the current block is read-only, do not need to enforce lock
  int k=0;

  // array of cl_event to wait until all buffer copy is finished
  cl_event *events = new cl_event[num_items];

  for (int i = 0; i < num_items; i++) {
    if (mask[i] != 0) {
      err = clEnqueueCopyBuffer(command, 
          data, masked_data,
          i*item_size, k*item_size,
          item_size, 
          0, NULL, events+k);

      if (err != CL_SUCCESS) {
        throw std::runtime_error(
            "Error in clEnqueueCopyBuffer()");
      }

      k++;
    }
  }
  err = clWaitForEvents(num_items, events);

  if (err != CL_SUCCESS) {
    throw std::runtime_error("Error during sampling");
  }

  for (int i = 0 ; i < num_items; i++) {
    clReleaseEvent(events[i]);
  }
  ocl_block->ready = true;

  delete [] events;

  return block;
#endif
}

} // namespace

