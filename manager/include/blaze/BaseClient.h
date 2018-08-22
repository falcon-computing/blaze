#ifndef BLAZE_BASECLIENT_H
#define BLAZE_BASECLIENT_H
#include <boost/asio.hpp>
#include <google/protobuf/message.h>
#include <string>
#include <utility>

#include "Common.h"

using namespace boost::asio;

namespace blaze {

class BaseClient {
 public:
  BaseClient(int port = 1027, std::string ip = "127.0.0.1");

  virtual void reset();

 protected:
  void connect();
  void send(::google::protobuf::Message &msg);
  void recv(::google::protobuf::Message &msg);

  // data structures for socket connection
  int          port_;
  std::string  ip_;
  bool         connected_;
  ios_ptr      ios_;
  endpoint_ptr endpoint_;
  socket_ptr   sock_;
};
} // namespace blaze
#endif
