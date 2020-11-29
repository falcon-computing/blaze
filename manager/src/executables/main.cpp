#include <arpa/inet.h>
#include <boost/system/error_code.hpp>
#include <boost/thread/thread.hpp>
#include <fcntl.h>
#include <fstream>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h> 
#include <sys/types.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <gflags/gflags.h>
#define LOG_HEADER "main"
#include <glog/logging.h>


#include "blaze/AppCommManager.h"
#include "blaze/BlockManager.h"
#include "blaze/CommManager.h"
#include "blaze/GAMCommManager.h"
#include "blaze/PlatformManager.h"
#include "blaze/QueueManager.h"
#include "blaze/TaskManager.h"

using namespace blaze;

class signal_exception : public std::runtime_error {
 public:
  signal_exception(): std::runtime_error("caught signal") {;}
};

void sigint_handler(int s) {
  throw signal_exception();
}

int main(int argc, char** argv) {

  // Initialize Google Flags
  google::SetVersionString(VERSION);
  google::SetUsageMessage(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);

  srand(time(NULL));

  if (argc < 2) {
    printf("USAGE: %s <conf_path>\n", argv[0]);
    return -1;
  }

  std::string conf_path(argv[1]);
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

  // configurations
  FLAGS_v      = conf->verbose();   // logging
  int app_port = conf->app_port();  // port for application
  int gam_port = conf->gam_port();  // port for GAM
  local_dir    = conf->local_dir(); // local dir for temp files

  gf_profile   = conf->profile();

  // create local dir
  try {
    local_dir += "/nam-" + getUid();
    if (!boost::filesystem::exists(local_dir)) {
      boost::filesystem::create_directories(local_dir);
    }
    DLOG(INFO) << "Set 'local_dir' to " << local_dir;
  } catch (boost::filesystem::filesystem_error &e) {
    LOG(ERROR) << "Failed to use '" << local_dir 
                 << "' as local directory, using '/tmp' instead.";
  }

  try {
  // setup PlatformManager
  PlatformManager platform_manager(conf);

  signal(SIGINT, sigint_handler);
  signal(SIGQUIT, sigint_handler);
  signal(SIGTRAP, sigint_handler);
  signal(SIGTERM, sigint_handler);

  // check all network interfaces on this computer, and 
  // open a communicator on each interface using the same port
  struct ifaddrs* ifAddrStruct = NULL;
  getifaddrs(&ifAddrStruct);

  // hold all pointers to the communicator
  std::vector<boost::shared_ptr<CommManager> > comm_pool;

  for (struct ifaddrs* ifa = ifAddrStruct; 
       ifa != NULL; 
       ifa = ifa->ifa_next) 
  {
    if (!ifa->ifa_addr) {
      continue;
    }
    // check if is a valid IP4 Address
    if (ifa->ifa_addr->sa_family == AF_INET) {

      // obtain the ip address as a string
      void* tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      char addressBuffer[INET_ADDRSTRLEN];

      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

      std::string ip_addr(addressBuffer);

      try {
        // create communicator for GAM
        boost::shared_ptr<CommManager> comm_gam( new GAMCommManager(
              &platform_manager, 
              ip_addr, gam_port)); 

        // create communicator for applications
        // it will start listening for new connections automatically
        boost::shared_ptr<CommManager> comm( new AppCommManager(
              &platform_manager, 
              ip_addr, app_port));

        VLOG(1) << "Start listening " << ip_addr << " on port " <<
          app_port << " and " << gam_port;

        // push the communicator pointer to pool to avoid object
        // being destroyed out of context
        comm_pool.push_back(comm);
        comm_pool.push_back(comm_gam);
      }
      catch (boost::system::system_error &e) {
        LOG_IF(WARNING, VLOG_IS_ON(1)) << "Failed to start communication manager on " 
                     << ip_addr << ", because: " << e.what();
      }
    }
  }
  if (comm_pool.empty()) {
    LOG(ERROR) << "Failed to start communication on any interface, exiting.";
    return 1;
  }

  while (1) {
    boost::this_thread::sleep_for(boost::chrono::seconds(5)); 
    //print_timers();
#ifndef NO_PROFILE
    if (gf_profile) {
      ksight::ksight.print(); 
    }
#endif
  }
  } // try
  catch (std::runtime_error const& ) {
    VLOG(1) << "Caught signal, cleaning up"; 
  }

  return 0;
}
