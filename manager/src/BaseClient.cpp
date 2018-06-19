#include <stdio.h>
#include <sstream>
#include <iomanip>

#include <glog/logging.h>
#include "blaze/BaseClient.h"

namespace blaze {

BaseClient::BaseClient(int port, std::string ip):
    port_(port), ip_(ip) 
{
  // setup socket connection
  ios_ptr ios(new io_service);
  endpoint_ptr endpoint(new ip::tcp::endpoint(
        ip::address::from_string(ip),
        port));

  ios_ = ios;
  endpoint_ = endpoint;

  socket_ptr sock(new ip::tcp::socket(*ios));
  try { 
    sock->connect(*endpoint);
    sock->set_option(ip::tcp::no_delay(true));

    sock_ = sock;
  }
  catch (const boost::system::system_error& e) {
    DLOG(ERROR) << "Cannot establish communication with " << ip << ":" << port;
    DLOG(ERROR) << "What: " << e.what();
  }
}

void BaseClient::send(::google::protobuf::Message &msg) {
  if (!sock_) {
    throw commError("No active socket connection.");
  }
  int msg_size = msg.ByteSize();

  sock_->send(
      boost::asio::buffer(
        reinterpret_cast<char*>(&msg_size), 
        sizeof(int)), 0);

  char* msg_data = new char[msg_size];
  msg.SerializeToArray(msg_data, msg_size);

  sock_->send(
      boost::asio::buffer(msg_data, msg_size),0);
}

void BaseClient::recv(::google::protobuf::Message &msg) {
  if (!sock_) {
    throw commError("No active socket connection.");
  }
  int msg_size = 0;

  sock_->receive(
      boost::asio::buffer(
        reinterpret_cast<char*>(&msg_size), 
        sizeof(int)), 0);

  if (msg_size <= 0) {
    throw commError(
        "Invalid message size of " +
        std::to_string((long long)msg_size));
  }
  // TODO: mysterious issue, for receiving the acc request message
  //boost::this_thread::sleep_for(boost::chrono::microseconds(5)); 

  char* msg_data = new char[msg_size];

  sock_->receive(
      boost::asio::buffer(msg_data, msg_size), 0);

  if (!msg.ParseFromArray(msg_data, msg_size)) {
    throw commError("Failed to parse input message");
  }

  delete [] msg_data;
}
} // namespace blaze
