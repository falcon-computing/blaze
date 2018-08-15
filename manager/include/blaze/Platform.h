#ifndef PLATFORM_H
#define PLATFORM_H

#include <gtest/gtest_prod.h>
#include <string>
#include <utility>

#include "acc_conf.pb.h"
#include "Common.h"
#include "TaskEnv.h"

namespace blaze {

class Platform {
  friend class PlatformManager;  
  FRIEND_TEST(PlatformTest, RemovePlatformTest);
  FRIEND_TEST(PlatformTest, ReopenPlatformTest);

public:
  Platform(std::map<std::string, std::string> &conf_table);
  // we want the correct destroyer for derived class 
  virtual ~Platform(); 

  virtual void addQueue(AccWorker &conf);
  virtual void removeQueue(std::string id);

  // store an accelerator setup on the platform
  //virtual void setupAcc(AccWorker &conf);

  // obtain a BlockManager
  virtual void createBlockManager(size_t cache_limit, size_t scratch_limit);

  virtual BlockManager* getBlockManager();

  virtual TaskManager_ref getTaskManager(std::string id);

  virtual QueueManager* getQueueManager();

  virtual void remove(int64_t block_id);

  // get an entry in the config_table matching the key
  std::string getConfig(std::string &key);

  // get TaskEnv to pass to Task
  virtual TaskEnv_ref getEnv();

protected:
  BlockManager_ptr block_manager;
  QueueManager_ptr queue_manager;

  // a table storing platform configurations mapped by key
  std::map<std::string, std::string> config_table;

  // a table storing platform configurations mapped by key
  std::map<std::string, AccWorker> acc_table;

  TaskEnv_ptr env;
};

} // namespace blaze
#endif
