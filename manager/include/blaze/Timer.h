#ifndef BLAZE_TIMER_H
#define BLAZE_TIMER_H

#include <gtest/gtest_prod.h>
#include <map>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "Common.h"

#ifndef TIMER_VERBOSE
#define TIMER_VERBOSE 2
#endif

#define TIMER_REPORT

namespace blaze {
extern std::map<std::string, uint64_t> g_total_time;
//extern std::map<std::string, uint64_t> g_last_time;

void print_timers();

uint64_t get_global_etime(std::string s);
// unit: us
void add_global_etime(std::string s, uint64_t t);
//uint64_t get_last_etime(std::string s);
//void add_last_etime(std::string s, uint64_t t);

class Timer {
 public: 
  Timer(std::string func = "-");
  ~Timer();
  void start();
  void stop();
  
 private:
  std::string func_;
  bool        auto_mode_;
  bool        started_;
  uint64_t    start_ts_;
};

#define CONCAT_FNAME(A, B) (std::string(A) + "::" + std::string(B))
#define CLASS_NAME typeid(*this).name()
#define FUNC_NAME CONCAT_FNAME(CLASS_NAME, __func__)

#define PLACE_TIMER Timer __timer_obj(FUNC_NAME);
#define PLACE_TIMER1(s) Timer __timer_obj(CONCAT_FNAME(FUNC_NAME, s));

} // namespace blaze
#endif
