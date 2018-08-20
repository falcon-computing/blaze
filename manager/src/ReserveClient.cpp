#ifdef NDEBUG
#define LOG_HEADER "ReserveClient"
#endif
#include <glog/logging.h>

#include "blaze/ReserveClient.h"

namespace blaze {

ReserveClient::ReserveClient(std::string acc_id, int port): 
      BaseClient(port, "127.0.0.1"), // reserve client only works for localhost
      alive_(false)
{
  connect();

  req_msg_.set_type(ACCRESERVE);
  req_msg_.set_acc_id(acc_id);

  if (!sock_) {
    VLOG(1) << "Cannot connect to manager, assuming reserved";
  }
  else {
    try {
      send(req_msg_);

      TaskMsg reply_msg;
      recv(reply_msg);

      if (reply_msg.type() == ACCGRANT) {
        VLOG(1) << "Successfully reserved accelerator " << acc_id;
        alive_ = true;

        // start heartbeat thread
        worker_.create_thread(
            boost::bind(&ReserveClient::heartbeat, this));
      }
      else {
        throw reserveError("request rejected");
      }
      VLOG(1) << "Reserving accelerator " << acc_id;
    } catch (commError &e) {
      throw reserveError("communication failed");
    }
  }
}

ReserveClient::~ReserveClient() {
  if (alive_) {
    alive_ = false;

    // need to wait for this thread to finish
    worker_.join_all();

    req_msg_.set_type(ACCFINISH);
    send(req_msg_);
  }

  DLOG(INFO) << "ReserveClient is destroyed";
}

void ReserveClient::heartbeat() {

  bool revoked = false;
  while (alive_) {
    try {
      send(req_msg_); 

      TaskMsg reply_msg;
      recv(reply_msg);

      if (reply_msg.type() != ACCGRANT) {
        LOG(INFO) << "Reservation is revoked";
        revoked = true;
        break;
      }
    }
    catch (commError &e) {
      DLOG(ERROR) << e.what();
      break;
    }
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
  } 

  // release environment
  // This should only happens if the reservation is revoked
  if (revoked) {
    this->release();
  }
}
} // namespace blaze
