#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <time.h>

#include <glog/logging.h>

#include "blaze/Task.h" 
#include "blaze/xlnx_opencl/OpenCLEnv.h" 

using namespace blaze;

class BWA_SW : public Task {
 public:
  // extends the base class constructor
  // to indicate how many input blocks
  // are required
  BWA_SW(): Task(1) {;}
 
  virtual void compute() {

    // dynamically cast the TaskEnv to OpenCLEnv
    OpenCLEnv* ocl_env = (OpenCLEnv*)getEnv();

    cl_kernel        kernel  = ocl_env->getKernel();
    cl_command_queue command = ocl_env->getCmdQueue();

    DLOG(INFO) << "not doing anthing";
  }
};

// define the constructor and destructor for dlopen()
extern "C" Task* create() {
  return new BWA_SW();
}

extern "C" void destroy(Task* p) {
  delete p;
}
