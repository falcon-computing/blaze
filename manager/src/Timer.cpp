#define LOG_HEADER "Timer"
#include <glog/logging.h>

#include "blaze/Common.h"
#include "blaze/Timer.h"

namespace blaze {

static std::map<std::string, uint64_t> g_total_time;
//std::map<std::string, uint64_t> g_last_time;

void print_timers() {
  for (auto p : g_total_time) {
    VLOG(TIMER_VERBOSE-1) << "Total time for " << p.first
      << " is " << p.second << " us";
  }
}

uint64_t get_global_etime(std::string s) {
  if (g_total_time.count(s)) return g_total_time[s];
  else return 0;
}

// unit: us
void add_global_etime(std::string s, uint64_t t) {
  if (g_total_time.count(s)) g_total_time[s] += t;
  else g_total_time[s] = t;
}

// unit: us
//uint64_t get_last_etime(std::string s) {
//  if (g_last_time.count(s)) return g_last_time[s];
//  else 0;
//}
//
//void add_last_etime(std::string s, uint64_t t) {
//  if (g_last_time.count(s)) g_last_time[s] += t;
//  else g_last_time[s] = t;
//}


Timer::Timer(std::string func): func_(func), auto_mode_(true)
{
  if (func.empty()) {
    throw invalidParam("Timer name cannot be empty");
  }
  start_ts_ = getUs(); 
}

Timer::~Timer() {
  if (auto_mode_) {
    uint64_t e_time = getUs() - start_ts_;
    VLOG(TIMER_VERBOSE) << func_ << " takes " << e_time << " us";
#ifdef TIMER_REPORT
    add_global_etime(func_, e_time);
    //add_last_etime(func_, e_time);
#endif
  }
}

void Timer::start() {
  auto_mode_ = false;
  if (started_) {
    LOG_IF(WARNING, VLOG_IS_ON(TIMER_VERBOSE)) 
      << "Timer " << func_ << " is already started";
  }
  start_ts_ = getUs();
}

void Timer::stop() {
  if (started_) {
    uint64_t e_time = getUs() - start_ts_;
    VLOG(TIMER_VERBOSE) << func_ << " takes " << e_time << " us";

#ifdef TIMER_REPORT
    add_global_etime(func_, e_time);
    //add_last_etime(func_, e_time);
#endif
  }
  started_ = false;
}
};
