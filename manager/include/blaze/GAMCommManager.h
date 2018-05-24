#ifndef BLAZE_GAM_COMM_MANAGER_H
#define BLAZE_GAM_COMM_MANAGER_H

#include <google/protobuf/message.h>
#include <gtest/gtest_prod.h>

#include "task.pb.h"
#include "Common.h"
#include "CommManager.h"

namespace blaze {

// Manager communication with GAM
class GAMCommManager : public CommManager {
  FRIEND_TEST(ConfigTests, CheckCommHandler);
public:
  GAMCommManager(
      PlatformManager* _platform,
      std::string address = "127.0.0.1",
      int ip_port = 1028
    ): CommManager(_platform, address, ip_port, 4) {;}
private:
  void process(socket_ptr);
  std::vector<std::pair<std::string, std::string> > last_labels;
};
} // namespace blaze
#endif
