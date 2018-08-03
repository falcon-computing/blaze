#include <glog/logging.h>
#include <gtest/gtest.h>

#include "TestCommon.h"

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  FLAGS_logtostderr = true;
  FLAGS_v = 1;
  return RUN_ALL_TESTS();
}
