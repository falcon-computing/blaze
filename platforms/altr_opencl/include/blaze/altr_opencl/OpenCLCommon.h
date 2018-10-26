#ifndef OPENCL_COMMON_H
#define OPENCL_COMMON_H

#include <boost/smart_ptr.hpp>
#include "blaze/Common.h"

namespace blaze {
class OpenCLBlock;
class OpenCLEnv;
class OpenCLKernelQueue;
class OpenCLPlatform;
class OpenCLQueueManager;

typedef boost::shared_ptr<OpenCLKernelQueue> OpenCLKernelQueue_ptr;

}

#endif
