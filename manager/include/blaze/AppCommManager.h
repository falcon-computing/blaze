#ifndef BLAZE_APP_COMM_MANAGER_H
#define BLAZE_APP_COMM_MANAGER_H

#include <boost/atomic.hpp>
#include <google/protobuf/message.h>
#include <gtest/gtest_prod.h>

#include "task.pb.h"
#include "Common.h"
#include "CommManager.h"

namespace blaze {

// Manage communication with Application
class AppCommManager : public CommManager 
{
  FRIEND_TEST(ConfigTests, CheckCommHandler);
public:
  AppCommManager(
      PlatformManager* _platform,
      std::string address = "127.0.0.1",
      int ip_port = 1027
    ): CommManager(_platform, address, ip_port, 24)
  {;}

private:
  void sendAccGrant(socket_ptr sock);

  void process(socket_ptr sock);
  void handleAccRegister(TaskMsg &msg);
  void handleAccDelete(TaskMsg &msg);
  void handleAccReserve(TaskMsg &msg, socket_ptr sock);

  void handleTaskTimeout(socket_ptr sock,
      std::string acc_id,
      Task_ptr task);

  std::map<std::string, boost::atomic<int> > 
    num_timeout_;
};
} // namespace blaze
#endif
