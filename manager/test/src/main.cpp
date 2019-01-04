#include <glog/logging.h>
#include <gtest/gtest.h>

#include "TestCommon.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  google::InitGoogleLogging(argv[0]);
  FLAGS_alsologtostderr = true;
  FLAGS_v = 2;
  return RUN_ALL_TESTS();
}
