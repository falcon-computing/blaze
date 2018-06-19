#ifndef BLAZE_RESERVECLIENT_H
#define BLAZE_RESERVECLIENT_H
#include <boost/asio.hpp>
#include <google/protobuf/message.h>
#include <string>
#include <utility>

#include "BaseClient.h"
#include "Common.h"
#include "task.pb.h"

namespace blaze {

class reserveError : public std::runtime_error {
public:
  explicit reserveError(const std::string& what_arg):
    std::runtime_error(what_arg) {;}
};

class ReserveClient : public BaseClient {
 public:
  ReserveClient(std::string acc_id, int port = 1027);
  ~ReserveClient();

 protected:
  virtual void release() {;}

 private:
  void heartbeat();

  bool     alive_;
  TaskMsg  req_msg_;

  boost::thread_group worker_;
};
} // namespace blaze
#endif
