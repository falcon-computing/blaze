#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <time.h>

#include <glog/logging.h>

#include "blaze/xlnx_opencl/OpenCLEnv.h" 
#include "PairHMMFpgaInterface.h"
#include "PairHMMTask.h"

#ifndef LOCAL_BLAZE
#include "blaze/Timer.h"
#else
#include "acc_lib/Timer.h"
double peak_kernel_gcups = 0;
double curr_kernel_gcups = 0;
#endif

PairHMM::PairHMM(): blaze::Task(3) 
{
  ;
}

PairHMM::~PairHMM() {
  if (env) {
    // push back the scratch blocks to TaskEnv
    env->putScratch("input",  input_);
    env->putScratch("output", output_);
  }
}

void PairHMM::prepare() {
  PLACE_TIMER;
  
  env = (blaze::OpenCLEnv*)getEnv();

  read_t* reads = NULL;
  hap_t*  haps  = NULL;

  num_cell = *((int*)getInput(0));

  num_read = deserialize(getInput(1), reads);
  num_hap  = deserialize(getInput(2), haps);

  // getting configurations and prepare fpga input
  int bank   = conf_str2int("bankID");
  int num_pe = conf_str2int("num_pe");
  int num_pu = num_pe / (READ_BLOCK_SIZE * HAP_BLOCK_SIZE);

  // get input block from scratch, create if it does not exists
  { PLACE_TIMER1("create input buffer");
  if (!env->getScratch("input", input_)) {
    PairHMMInput_ptr input(new PairHMMInput(env, bank));
    input_ = input;
    DLOG(INFO) << "Creating a new PairHMMInput";
  }
  else {
    DLOG(INFO) << "Getting a new PairHMMInput from Scratch";
  }
  }

  pack_fpga_input(num_pu, num_read, num_hap, reads, haps, input_->bundle);
  free_reads(reads, num_read);
  free_haps(haps, num_hap);

  // allocate output buffer
  blaze::ConfigTable_ptr conf(new blaze::ConfigTable());
  conf->write_conf("bankID", bank);

  cl_mem out_cl_buf;

  { PLACE_TIMER1("create output buffer");
  if (!env->getScratch("output", output_)) {
    uint64_t output_size = MAX_RSDATA_NUM * MAX_HAPDATA_NUM;
    blaze::DataBlock_ptr b = env->create_block(1, 
        output_size, output_size*sizeof(float), 
        0, blaze::DataBlock::OWNED, conf);

    output_ = b;
  }
  setOutput(0, output_);
  }

  cl_int err = 0;

  cl_command_queue command = env->getCmdQueue();

  if (!command) {
    DLOG(ERROR) << "failed to get OpenCLEnv";
    throw blaze::invalidParam(__func__);
  }
  {
    PLACE_TIMER1("write_buffer");
    cl_event event;
    cl_int err = clEnqueueMigrateMemObjects(command, 1, &input_->buf, 0, 0, NULL, &event);
    if (err != CL_SUCCESS) {
      throw std::runtime_error("failed to migrate input buffer");
    }
    clWaitForEvents(1, &event);
    clReleaseEvent(event);
  }
}

void PairHMM::compute() {
  PLACE_TIMER;

  cl_kernel        kernel  = env->getKernel();
  cl_command_queue command = env->getCmdQueue();

  clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_->buf);
  clSetKernelArg(kernel, 1, sizeof(int), &num_read);
  clSetKernelArg(kernel, 2, sizeof(int), &num_hap);
  clSetKernelArg(kernel, 3, sizeof(cl_mem), (cl_mem*)output_->getData());

  uint64_t k_start = blaze::getUs();

  { PLACE_TIMER1("kernel");
  cl_event event;
  cl_int err = clEnqueueTask(command, kernel, 0, NULL, &event);
  if (err != CL_SUCCESS) {
    throw std::runtime_error("failed to enqueue kernel");
  }
  clWaitForEvents(1, &event);
  clReleaseEvent(event);
  }

#ifdef LOCAL_BLAZE
  double gcups = (double)num_cell / (blaze::getUs() - k_start) / 1e3;
  curr_kernel_gcups = gcups;
  peak_kernel_gcups = std::max(gcups, peak_kernel_gcups);
#endif
}
