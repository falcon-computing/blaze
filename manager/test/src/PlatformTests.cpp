#include "TestCommon.h"
#include "blaze/Platform.h"
#include "blaze/PlatformManager.h"

namespace blaze {


void addLoopBack(AccPlatform* platform) {
  AccWorker *worker = platform->add_acc();
  worker->set_id("loopBack");
  worker->set_path(pathToLoopBack());
}

void addArrayTest(AccPlatform* platform) {
  AccWorker *worker = platform->add_acc();
  worker->set_id("arrayTest");
  worker->set_path(pathToArrayTest());
}

TEST_F(PlatformTests, RegisterPlatformTest) {

  // config manager to use default CPU platform
  ManagerConf conf;
  AccPlatform *platform = conf.add_platform();
  
  // add two AccWorkers
  addLoopBack(platform);
  addArrayTest(platform);

  PlatformManager platform_manager(&conf);
  ASSERT_TRUE(platform_manager.accExists("loopBack"));
  ASSERT_TRUE(platform_manager.accExists("arrayTest"));

  ASSERT_TRUE(platform_manager.platformExists("cpu"));
  ASSERT_EQ("cpu", platform_manager.acc_table["loopBack"]);
  ASSERT_EQ("cpu", platform_manager.acc_table["arrayTest"]);

  ASSERT_LT(0, platform_manager.platform_conf_table.count("cpu"));
  ASSERT_EQ(0, platform_manager.acc_conf_table.count("cpu"));
  //ASSERT_EQ(0, platform_manager.acc_conf_table["cpu"].size());
}

TEST_F(PlatformTests, RemovePlatformTest) {
  // config manager to use default CPU platform
  ManagerConf conf;
  AccPlatform *platform = conf.add_platform();
  
  // add two AccWorkers
  addLoopBack(platform);
  addArrayTest(platform);

  PlatformManager platform_manager(&conf); 

  platform_manager.removePlatform("cpu");
  ASSERT_FALSE(platform_manager.platformExists("cpu"));
  ASSERT_FALSE(platform_manager.accExists("loopBack"));
  ASSERT_FALSE(platform_manager.accExists("arrayTest"));

  ASSERT_LT(0, platform_manager.platform_conf_table.count("cpu"));
  ASSERT_LT(0, platform_manager.acc_conf_table.count("cpu"));
  ASSERT_EQ(2, platform_manager.acc_conf_table["cpu"].size());
}

TEST_F(PlatformTests, ReopenPlatformTest) {
  // config manager to use default CPU platform
  ManagerConf conf;
  AccPlatform *platform = conf.add_platform();
  
  // add two AccWorkers
  addLoopBack(platform);
  addArrayTest(platform);

  PlatformManager platform_manager(&conf); 

  platform_manager.removePlatform("cpu");
  platform_manager.openPlatform("cpu");

  ASSERT_TRUE(platform_manager.accExists("loopBack"));
  ASSERT_TRUE(platform_manager.accExists("arrayTest"));

  ASSERT_TRUE(platform_manager.platformExists("cpu"));
  ASSERT_EQ("cpu", platform_manager.acc_table["loopBack"]);
  ASSERT_EQ("cpu", platform_manager.acc_table["arrayTest"]);

  ASSERT_LT(0, platform_manager.platform_conf_table.count("cpu"));
  ASSERT_EQ(0, platform_manager.acc_conf_table.count("cpu"));
  ASSERT_EQ(0, platform_manager.acc_conf_table["cpu"].size());
  DLOG(INFO) << "Test finished";
}

} // namespace blaze
