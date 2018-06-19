#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <time.h>

#include <glog/logging.h>

#include "blaze/Task.h" 
#include "blaze/xlnx_opencl/OpenCLEnv.h" 

using namespace blaze;

double total_time = 0;
struct timespec diff_time(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

class PairHMM : public Task {
 public:
  // extends the base class constructor
  // to indicate how many input blocks
  // are required
#define NUM_ARGS 7
  PairHMM(): Task(NUM_ARGS) {;}
  
  virtual uint64_t estimateClientTime(){
    float cells = *((float*)getInput(NUM_ARGS - 1));
    float AVX_GCUPS = 0.55;
    return (uint64_t)(cells / AVX_GCUPS);
  }

  virtual uint64_t estimateTaskTime(){
    float cells = *((float*)getInput(NUM_ARGS - 1));
    float FPGA_GCUPS = 3.3;
    return (uint64_t)(cells / FPGA_GCUPS + 2 * 600000);
  }

  virtual void compute() {

    // dynamically cast the TaskEnv to OpenCLEnv
    OpenCLEnv* ocl_env = (OpenCLEnv*)getEnv();

    cl_kernel        kernel  = ocl_env->getKernel();
    cl_command_queue command = ocl_env->getCmdQueue();

    int err;
    cl_event event;
    size_t global[1], local[1];
    global[0] = 1;
    local[0] = 1;

    struct timespec time1, time2, diff;
    double time_elapse = 0;

    cl_mem input0 = *((cl_mem*)getInput(0));
    int    numRead0 = *((int*)getInput(1));
    int    numHap0 = *((int*)getInput(2));
    cl_mem input1 = *((cl_mem*)getInput(3));
    int    numRead1 = *((int*)getInput(4));
    int    numHap1 = *((int*)getInput(5));
    cl_mem input2 = NULL;
    int    numRead2 = 0;
    int    numHap2 = 0;

    std::string bankid("bankID");

    cl_mem output0 = *((cl_mem*)getOutput(0, numRead0 * numHap0, 1, 
          sizeof(float), std::make_pair(bankid, 1)));
    cl_mem output1 = *((cl_mem*)getOutput(1, numRead1 * numHap1, 1, 
          sizeof(float), std::make_pair(bankid, 2)));
    cl_mem output2 = NULL;

    DLOG(INFO) << "core0: numRead = " << numRead0 << ", numHap = " << numHap0;
    DLOG(INFO) << "core1: numRead = " << numRead1 << ", numHap = " << numHap1;

    if (!input0 || !output0 || !input1 || !output1) {
      throw std::runtime_error("Buffer are not allocated");
    }

    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input0);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &numRead0);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &numHap0);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &output0);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &input1);
    err |= clSetKernelArg(kernel, 5, sizeof(int), &numRead1);
    err |= clSetKernelArg(kernel, 6, sizeof(int), &numHap1);
    err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), &output1);
    err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), &input2);
    err |= clSetKernelArg(kernel, 9, sizeof(int), &numRead2);
    err |= clSetKernelArg(kernel,10, sizeof(int), &numHap2);
    err |= clSetKernelArg(kernel,11, sizeof(cl_mem), &output2);

    if (err != CL_SUCCESS) {
      throw std::runtime_error("Failed to set args!");
    }

    clock_gettime(CLOCK_REALTIME, &time1);

    err = clEnqueueNDRangeKernel(command, kernel, 1, NULL, (size_t*)&global, (size_t*)&local, 0, NULL, &event);
    if (err) {
      throw std::runtime_error("Failed to execute kernel!");
    }

    clWaitForEvents(1, &event);
    clock_gettime(CLOCK_REALTIME, &time2);

    diff = diff_time(time1, time2);
    time_elapse += diff.tv_sec + diff.tv_nsec * 1e-9;
    total_time += time_elapse;
    clReleaseEvent(event);
    DLOG(INFO) << "Kernel time is " << time_elapse << " secs, "
               << "total time is " << total_time << " secs";
  }
};

// define the constructor and destructor for dlopen()
extern "C" Task* create() {
  return new PairHMM();
}

extern "C" void destroy(Task* p) {
  delete p;
}
