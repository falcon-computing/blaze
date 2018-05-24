#ifndef PLATFORM_MANAGER_H
#define PLATFORM_MANAGER_H

#include <boost/smart_ptr.hpp>
#include <gtest/gtest_prod.h>
#include <map>
#include <string>
#include <vector>

#include "acc_conf.pb.h"
#include "Common.h"

namespace blaze {

class PlatformManager 
: public boost::basic_lockable_adapter<boost::mutex>
{
  friend class AppCommManager;
  FRIEND_TEST(PlatformTests, RegisterPlatformTest);
  FRIEND_TEST(PlatformTests, RemovePlatformTest);
  FRIEND_TEST(PlatformTests, ReopenPlatformTest);

public:
  PlatformManager(ManagerConf *conf);
  ~PlatformManager();

  bool accExists(std::string acc_id);
  bool platformExists(std::string platform);

  std::string getPlatformIdByAccId(std::string acc_id);

  Platform* getPlatformByAccId(std::string acc_id);

  Platform* getPlatformById(std::string platform_id);

  TaskManager_ref getTaskManager(std::string acc_id);

  // remove a shared block from all platforms
  void removeShared(int64_t block_id);

  std::vector<std::pair<std::string, std::string> > getLabels();

private:
  // create a new platform from file
  Platform_ptr create(
      std::string id, 
      std::map<std::string, std::string> &conf_table);

  void registerAcc(
      std::string platform_id, 
      AccWorker &acc_conf);

  void removeAcc(
      std::string requester,
      std::string acc_id,
      std::string platform_id);

  void registerPlatform(AccPlatform conf);
  void openPlatform(std::string platform_id);
  void removePlatform(std::string platform_id);

  // map platform_id to Platform 
  std::map<std::string, Platform_ptr> platform_table;

  // map acc_id to accelerator platform
  std::map<std::string, std::string> acc_table;

  // map platform_id to platform configuration, in which
  // all AccWorkers will be ignored.
  // AccWorkers will be stored in a separate table.
  std::map<std::string, AccPlatform> platform_conf_table;

  // map platform_id to a list of AccWorker configurations,
  // used to restore all registered accelerators when platform
  // is re-created
  std::map<std::string, std::vector<AccWorker> > acc_conf_table;
};
} // namespace blaze
#endif
