#ifndef BLAZE_CONF_TABLE_H
#define BLAZE_CONF_TABLE_H

#include <boost/any.hpp>
#include <map>
#include <string>

#include "Common.h"

namespace blaze {
class ConfigTable {
public:

  template<typename T> 
  bool get_conf(std::string key, T &val) {
    if (!table_.count(key)) return false;
    try {
      val = boost::any_cast<T>(table_[key]);
    }
    catch (boost::bad_any_cast &e) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "failed to convert value for " 
        << key;
      return false;
    }
    return true;
  }

  template<typename T> 
  T get_conf(std::string key) {
    T ret;  
    if (get_conf(key, ret)) {
      return ret;
    }
    else {
      throw invalidParam("invalid conf key: " + key);
    }
  }

  template<typename T>
  bool write_conf(std::string key, T &val, bool over_write = false) {
    if (over_write && table_.count(key)) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "The configuration for "  << key
          << " is already set";
      return false;
    }
    table_[key] = val;
  }

private:
  std::map<std::string, boost::any> table_;
};

const ConfigTable_ptr NULL_ConfigTable_ptr;
} // namespace blaze
#endif
