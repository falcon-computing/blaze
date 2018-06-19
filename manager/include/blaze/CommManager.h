#ifndef BLAZE_COMM_MANAGER_H
#define BLAZE_COMM_MANAGER_H

#include <google/protobuf/message.h>
#include <gtest/gtest_prod.h>

#include "task.pb.h"
#include "Common.h"

using namespace boost::asio;

namespace blaze {

class AccReject : public std::logic_error {
public:
  explicit AccReject(const std::string& what_arg):
    std::logic_error(what_arg) {;}
};

class AccFailure : public std::logic_error {
public:
  explicit AccFailure(const std::string& what_arg):
    std::logic_error(what_arg) {;}
};

/*
 * Communicator design for Node Manager
 */
class CommManager
: public boost::basic_lockable_adapter<boost::mutex>
{
  FRIEND_TEST(ConfigTests, CheckCommHandler);

public:
  CommManager(
      PlatformManager* _platform,
      std::string address = "127.0.0.1",
      int ip_port = 1027,
      int _max_threads = boost::thread::hardware_concurrency());

  ~CommManager();

protected:
  // pure virtual method called by listen
  virtual void process(socket_ptr) = 0;

  // reference to platform manager
  PlatformManager *platform_manager;

private:
  void startAccept();
  void handleAccept(
      const boost::system::error_code& error,
      socket_ptr socket);

  int           srv_port;
  std::string   ip_address;

  ios_ptr       ios;
  endpoint_ptr  endpoint;
  acceptor_ptr  acceptor;
  boost::thread_group comm_threads;
};

} // namespace blaze
#endif
