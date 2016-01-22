#include <stdio.h>
#include <time.h>
#include <fcntl.h>   
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stdexcept>
#include <cstdint>

#define LOG_HEADER "GAMCommManager"
#include <glog/logging.h>

#include "proto/msgGamNam.pb.h"
#include "CommManager.h"
#include "PlatformManager.h"

namespace blaze {

void GAMCommManager::process(socket_ptr sock) {

  // turn off Nagle Algorithm to improve latency
  sock->set_option(ip::tcp::no_delay(true));

  // set socket buffer size to be 4MB
  socket_base::receive_buffer_size option(4*1024*1024);
  sock->set_option(option); 

  Gam2NamRequest msg;
  try {
    recv(msg, sock);
    
    if (msg.type() == Gam2NamRequest::ACCNAMES) {
      
      Nam2GamAccNames reply_msg;

      // compile a list from platform manager
      std::vector<std::pair<std::string, std::string> > labels = 
        platform_manager->getLabels();

      // sort list to avoid permutations
      //std::sort(labels.begin(), labels.end());

<<<<<<< HEAD
      if ((!msg.has_pull() || !msg.pull()))
=======
      if ((!msg.has_pull() || !msg.pull()) && 
          labels == last_labels) 
>>>>>>> b1c71b8b5bd2e6244da4090975b65e18704e1d76
      {
        reply_msg.set_isupdated(false); 
      }
      else {
        reply_msg.set_isupdated(true); 
      
        for (int i=0; i<labels.size(); i++) {
          Accelerator* label = reply_msg.add_acc_names();
          label->set_acc_name(labels[i].first);
          label->set_device_name(labels[i].second);

          DLOG(INFO) << "Add acc name: " << labels[i].first << 
            " | " << labels[i].second;
        }
<<<<<<< HEAD
=======
        last_labels = labels;
>>>>>>> b1c71b8b5bd2e6244da4090975b65e18704e1d76
      }

      // send reply message
      send(reply_msg, sock);

      LOG_EVERY_N(INFO, 60) << "Sent 60 ACCNAMES to GAM";
    }
    else {
      throw std::runtime_error("Unexpected message");
    }
  }
  catch (std::exception &e) {
    LOG(ERROR) << "Failed to communicate with GAM: " << e.what();
  }
}
} // namespace blaze

