#include <algorithm>
#include <boost/thread.hpp>
#include <vector>
#include <sstream>
#include <string>

#include "blaze/Task.h" 
using namespace blaze;

// delay with estimation
class DelayWEst : public Task {
public:

  // extends the base class constructor
  // to indicate how many input blocks
  // are required
  DelayWEst(): Task(3) {;}

  virtual uint64_t estimateTaskTime(){
    uint64_t delay = *((uint64_t*)getInput(0));
    return delay*1e6;
  }

  virtual uint64_t estimateClientTime(){
    uint64_t delay = *((uint64_t*)getInput(1));
    return delay*1e6;
  }

  // overwrites the compute function
  virtual void compute() {
    uint64_t delay       = *((uint64_t*)getInput(0));
    uint64_t cpu_delay   = *((uint64_t*)getInput(1));
    uint64_t force_delay = *((uint64_t*)getInput(2));

    if (force_delay == 0) {
      VLOG(1) << "delay for " << delay << " ms";
      boost::this_thread::sleep_for(
          boost::chrono::milliseconds(delay)); 
    }
    else {
      VLOG(1) << "delay for " << force_delay << " ms";
      boost::this_thread::sleep_for(
          boost::chrono::milliseconds(force_delay)); 
    }
  }
};

extern "C" Task* create() {
  return new DelayWEst();
}

extern "C" void destroy(Task* p) {
  delete p;
}
