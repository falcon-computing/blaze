#include "TestCommon.h"
#include "blaze/AppCommManager.h"
#include "blaze/Client.h"
#include "blaze/CommManager.h"
#include "blaze/GAMCommManager.h"
#include "blaze/PlatformManager.h"
#include "acc_conf.pb.h"

namespace blaze {

TEST_F(ConfigTests, CheckCommHandler) {

  // settings
  std::string acc_id      = "test";
  std::string platform_id = "cpu";

  // config manager to use default CPU platform
  ManagerConf conf;
  AccPlatform *platform = conf.add_platform();

  // start manager
  PlatformManager platform_manager(&conf);
  boost::shared_ptr<AppCommManager> comm( new AppCommManager(
        &platform_manager, "127.0.0.1", app_port)); 

  TaskMsg req_msg;

  // test acc register
  req_msg.set_type(ACCREGISTER);

  AccMsg* acc_msg = req_msg.mutable_acc();

  acc_msg->set_acc_id(acc_id);
  acc_msg->set_platform_id(platform_id);
  acc_msg->set_task_impl(pathToLoopBack());

  try {
    comm->handleAccRegister(req_msg);
  } catch (std::exception &e) {
    ASSERT_EQ(true, false) << "Caught unexpected exception: " << e.what();
  }

  // accelerator should be registered
  ASSERT_EQ(true, platform_manager.accExists(acc_id));
  ASSERT_EQ(true, runLoopBack());

  // try register another one
  acc_msg->set_task_impl(pathToArrayTest());
  try {
    comm->handleAccRegister(req_msg);
    ASSERT_TRUE(false) << "Exception should be called";
  } catch (std::exception &e) {
    ASSERT_TRUE(true) << "Caught unexpected exception: " << e.what();
  }

  // accelerator should not change
  ASSERT_EQ(true, runLoopBack());

  // test acc delete
  req_msg.set_type(ACCDELETE);

  try {
    comm->handleAccDelete(req_msg);
  } catch (std::exception &e) {
    ASSERT_EQ(true, false) << "Caught unexpected exception: " << e.what();
  }
  ASSERT_EQ(false, platform_manager.accExists(acc_id));

  // should be false since accelerator is deleted
  ASSERT_EQ(false, runLoopBack());

  // test acc register again for ArrayTest
  req_msg.set_type(ACCREGISTER);

  try {
    comm->handleAccRegister(req_msg);
  } catch (std::exception &e) {
    ASSERT_TRUE(false) << "Caught unexpected exception: " << e.what();
  }
  // accelerator should be registered as ArrayTest
  ASSERT_EQ(true, platform_manager.accExists(acc_id));
  ASSERT_EQ(true, runArrayTest());

  LOG(INFO) << "Finished ConfigTest";
}

} // namespace blaze
