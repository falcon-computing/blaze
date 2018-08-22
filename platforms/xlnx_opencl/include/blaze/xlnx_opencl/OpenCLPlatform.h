#ifndef OPENCL_PLATFORM_H
#define OPENCL_PLATFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdexcept>

#include <CL/opencl.h>

#include "OpenCLCommon.h"
#include "OpenCLBlock.h"
#include "OpenCLEnv.h"
#include "blaze/Platform.h"

namespace blaze {

#define checkCLRun(cmd) { \
  cl_int err = cmd; \
  if (err != CL_SUCCESS) \
    DLOG(ERROR) << #cmd << " failed"; \
  DLOG(INFO) << #cmd << " succeeded"; \
}

class OpenCLPlatform : public Platform {

public:
  OpenCLPlatform(std::map<std::string, std::string> &conf_table);

  virtual ~OpenCLPlatform();

  virtual TaskEnv_ref getEnv();

  virtual void createBlockManager(size_t cache_limit, size_t scratch_limit);
  virtual BlockManager* getBlockManager();

  void addQueue(AccWorker &conf);
  void removeQueue(std::string id);

  void changeProgram(std::string acc_id);

  cl_program& getProgram();

private:
  int load_file(const char* filename, char** result);
  
  OpenCLEnv*  env;
  TaskEnv_ptr env_ptr;

  std::string curr_acc_id;
  cl_program  curr_program;

  std::map<std::string, std::pair<int, unsigned char*> > bitstreams;
  //std::map<std::string, cl_kernel>  kernels;
};

extern "C" Platform* create(
    std::map<std::string, std::string> &config_table);

extern "C" void destroy(Platform* p);

} // namespace blaze

#endif
