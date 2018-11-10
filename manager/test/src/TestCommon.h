#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <unistd.h>
#include <dlfcn.h>

#include <cstdint>
#include <string>
#include <stdexcept>

#include <boost/thread/thread.hpp>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "blaze/Common.h"

namespace blaze {

// custom exceptions
class cpuCalled : public std::runtime_error {
public:
  explicit cpuCalled(const std::string& what_arg):
    std::runtime_error(what_arg) {;}
};

class BlockTests : public ::testing::Test {
  protected:
    BlockTests(); 
    virtual void SetUp();
    virtual void TearDown();

    Platform_ptr  platform;
    BlockManager* bman;
};

class ClientTests : public ::testing::Test {
  protected:
    ClientTests() { }
};

class ConfigTests : public ::testing::Test {
  protected:
    ConfigTests() { }
};

class PlatformTests : public ::testing::Test {
  protected:
    PlatformTests() { }
};

// worker function to run app tests using Client
bool runArrayTest();
bool runLoopBack(int data_size = 1024);
bool runDelay(int data_size = 1024);
bool runDelayWEst(uint64_t task_us, uint64_t cpu_us, uint64_t force_us = 0);

inline std::string get_absolute_path(std::string path) {
  boost::filesystem::wpath file_path(path);
  if (boost::filesystem::exists(file_path)) {
    return boost::filesystem::canonical(file_path).string();
  }
  else {
    if (file_path.is_absolute()) {
      return path;
    }
    else {
      file_path = boost::filesystem::current_path() / file_path;
      return file_path.string();
    }
  }
}

inline std::string get_bin_dir() {
  std::stringstream ss;
  ss << "/proc/" << getpid() << "/exe";

  boost::filesystem::wpath bin_path(get_absolute_path(ss.str()));
  return bin_path.parent_path().string();
}

// global variables
static int app_port = 7777;
//std::string platform_dir = "../../platforms";
//std::string nv_opencl_path  = platform_dir + "/nv_opencl/nv_opencl.so";
//std::string lnx_opencl_path = platform_dir + "/xlnx_opencl/xlnx_opencl.so";

inline std::string pathToArrayTest() {
  return get_bin_dir() + "/tasks/libarrayTest.so";
}

inline std::string pathToDelay() {
  return get_bin_dir() + "/tasks/libdelay.so";
}

inline std::string pathToLoopBack() {
  return get_bin_dir() + "/tasks/libloopBack.so";
}

inline std::string pathToDelayWEst() {
  return get_bin_dir() + "/tasks/libdelay_w_est.so";
}


} // namespace blaze
#endif
