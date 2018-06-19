#include <stdlib.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#define LOG_HEADER "Admin"
#include <glog/logging.h>

// use flexlm
#ifdef USELICENSE
#include "falcon-lic/license.h"
#endif

#include "blaze/Admin.h"

using namespace blaze;

int main(int argc, char** argv) {

  google::InitGoogleLogging(argv[0]);

  FLAGS_logtostderr = 1;
  FLAGS_v = 1;

#ifdef USELICENSE
  namespace fc   = falconlic;
#if DEPLOYMENT == aws
  fc::enable_aws();
#elif DEPLOYMENT == hwc
  fc::enable_hwc();
#endif
  fc::enable_flexlm();

  namespace fclm = falconlic::flexlm;
  fclm::add_feature(fclm::FALCON_DNA);
  int licret = fc::license_verify();
  if (licret != fc::SUCCESS) {
    LOG(ERROR) << "Cannot authorize software usage: " << licret;
    LOG(ERROR) << "Please contact support@falcon-computing.com for details.";
    return licret;
  }
#endif

  srand(time(NULL));

  if (argc < 3) {
    printf("USAGE: %s <r|d> <conf_path> [NAM_IP]\n", argv[0]);
    return -1;
  }

  std::string conf_path(argv[2]);
  int file_handle = open(conf_path.c_str(), O_RDONLY);

  if (file_handle < 0) {
    printf("cannot find configure file: %s\n",
        argv[1]);
    return -1;
  }
  
  ManagerConf *conf = new ManagerConf();
  google::protobuf::io::FileInputStream fin(file_handle);
  

  if (!google::protobuf::TextFormat::Parse(&fin, conf)) {
    LOG(FATAL) << "cannot parse configuration from " << argv[1];
  }

  Admin admin;

  if (strcmp(argv[1], "d")) {
    admin.registerAcc(*conf);
  }
  else {
    admin.deleteAcc(*conf);
  }

#ifdef USELICENSE
  // release license
  fc::license_clean();
#endif

  return 0;
}
