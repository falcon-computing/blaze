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
#define NUM_ARGS 10
  PairHMM(): Task(NUM_ARGS) {;}
  
  virtual uint64_t estimateClientTime(){
    float cells = *((float*)getInput(NUM_ARGS - 1));
    float AVX_GCUPS = 0.3;
    return (uint64_t)(cells / AVX_GCUPS);
  }

  virtual uint64_t estimateTaskTime(){
    float cells = *((float*)getInput(NUM_ARGS - 1));
    float FPGA_GCUPS = 19;
    float PCIe_BW = 2.0; //GBps
    float PCIe_LAT= 300 * 1e3; //ns
    float CLBufferCreateTime = 0.7 * 1e6; //ns
    float read_mm_time = 1e6;
    return (uint64_t)(cells / FPGA_GCUPS + 3 * (CLBufferCreateTime + read_mm_time + 3e6 / PCIe_BW + 2 * PCIe_LAT));
  }

  virtual void compute() {

    // dynamically cast the TaskEnv to OpenCLEnv
    OpenCLEnv* ocl_env = (OpenCLEnv*)getEnv();

    int err;
    cl_kernel       kernel0  = ocl_env->getKernel();
    cl_command_queue command = ocl_env->getCmdQueue();
    cl_program       program = ocl_env->getProgram();
    cl_kernel kernel1 = clCreateKernel(program, "pmm_core_top1", &err);
    if((!kernel1) || err != CL_SUCCESS){
        throw std::runtime_error("Failed to create compute kernel for pairhmm core1!");
    }
    cl_kernel kernel2 = clCreateKernel(program, "pmm_core_top2", &err);
    if((!kernel2) || err != CL_SUCCESS){
        throw std::runtime_error("Failed to create compute kernel for pairhmm core2!");
    }

    cl_event kernel_event[3];

    struct timespec time1, time2, diff;
    double time_elapse = 0;

    cl_mem input0   = *((cl_mem*)getInput(0));
    int    numRead0 = *((int*)getInput(1));
    int    numHap0  = *((int*)getInput(2));
    cl_mem input1   = *((cl_mem*)getInput(3));
    int    numRead1 = *((int*)getInput(4));
    int    numHap1  = *((int*)getInput(5));
    cl_mem input2   = *((cl_mem*)getInput(6));
    int    numRead2 = *((int*)getInput(7));
    int    numHap2  = *((int*)getInput(8));

    std::string bankid("bankID");

    cl_mem output0 = *((cl_mem*)getOutput(0, numRead0 * numHap0, 1, 
          sizeof(float), std::make_pair(bankid, 3)));
    cl_mem output1 = *((cl_mem*)getOutput(1, numRead1 * numHap1, 1, 
          sizeof(float), std::make_pair(bankid, 1)));
    cl_mem output2 = *((cl_mem*)getOutput(2, numRead2 * numHap2, 1, 
          sizeof(float), std::make_pair(bankid, 0)));

    DLOG(INFO) << "core0: numRead = " << numRead0 << ", numHap = " << numHap0;
    DLOG(INFO) << "core1: numRead = " << numRead1 << ", numHap = " << numHap1;
    DLOG(INFO) << "core2: numRead = " << numRead2 << ", numHap = " << numHap2;

    if (!input0 || !output0 || 
        !input1 || !output1 ||
        !input2 || !output2) {
      throw std::runtime_error("Buffer are not allocated");
    }

    err  = clSetKernelArg(kernel0, 0, sizeof(cl_mem), &input0);
    err |= clSetKernelArg(kernel0, 1, sizeof(int), &numRead0);
    err |= clSetKernelArg(kernel0, 2, sizeof(int), &numHap0);
    err |= clSetKernelArg(kernel0, 3, sizeof(cl_mem), &output0);
    err |= clSetKernelArg(kernel1, 0, sizeof(cl_mem), &input1);
    err |= clSetKernelArg(kernel1, 1, sizeof(int), &numRead1);
    err |= clSetKernelArg(kernel1, 2, sizeof(int), &numHap1);
    err |= clSetKernelArg(kernel1, 3, sizeof(cl_mem), &output1);
    err |= clSetKernelArg(kernel2, 0, sizeof(cl_mem), &input2);
    err |= clSetKernelArg(kernel2, 1, sizeof(int), &numRead2);
    err |= clSetKernelArg(kernel2, 2, sizeof(int), &numHap2);
    err |= clSetKernelArg(kernel2, 3, sizeof(cl_mem), &output2);

    if (err != CL_SUCCESS) {
      throw std::runtime_error("Failed to set args!");
    }
    
    clock_gettime(CLOCK_REALTIME, &time1);
    
    err = clEnqueueTask(command, kernel0, 0, NULL, &kernel_event[0]);
    if (err) {
      throw std::runtime_error("Failed to execute kernel0!");
    }
    err = clEnqueueTask(command, kernel1, 0, NULL, &kernel_event[1]);
    if (err) {
      throw std::runtime_error("Failed to execute kernel1!");
    }
    err = clEnqueueTask(command, kernel2, 0, NULL, &kernel_event[2]);
    if (err) {
      throw std::runtime_error("Failed to execute kernel2!");
    }

    clWaitForEvents(3, kernel_event);
    clock_gettime(CLOCK_REALTIME, &time2);

    diff = diff_time(time1, time2);
    time_elapse += diff.tv_sec + diff.tv_nsec * 1e-9;
    total_time += time_elapse;
    clReleaseEvent(kernel_event[0]);
    clReleaseEvent(kernel_event[1]);
    clReleaseEvent(kernel_event[2]);
    clReleaseKernel(kernel1);
    clReleaseKernel(kernel2);
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
