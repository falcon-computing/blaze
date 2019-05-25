#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <syscall.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cstdint>
#include <string>
#include <stdexcept>

#ifdef NDEBUG
#define LOG_HEADER "Common"
#endif
#include <glog/logging.h>

#include "blaze/Common.h"

namespace blaze {

  bool gf_profile = false;

  uint64_t getUs() {
    struct timespec tr;
    clock_gettime(CLOCK_REALTIME, &tr);

    return (uint64_t)tr.tv_sec*1e6 + tr.tv_nsec/1e3;
  }

  uint64_t getMs() {
    struct timespec tr;
    clock_gettime(CLOCK_REALTIME, &tr);

    return (uint64_t)tr.tv_sec*1e3 + tr.tv_nsec/1e6;
  }

  // get timestamp
  std::string getTS() {

    struct timespec tr;
    clock_gettime(CLOCK_REALTIME, &tr);
    struct tm *l_time = localtime(&tr.tv_sec);
    char t_str[100];
    strftime(t_str, sizeof(t_str), "%Y%m%d-%H%M%S", l_time);

    std::string ts_str(t_str);
    std::string us_str = std::to_string((long long int)tr.tv_nsec/1000);

    int num_zero = 6 - us_str.size();
    for(int i = 0; i < num_zero; ++i) {
      us_str = "0" + us_str;
    }

    ts_str += us_str;

    return ts_str;
  }

  // get current thread id
  // using the same code from googlelog/src/utilities.cc
  // without OS checking
  uint32_t getTid() {
    static bool lacks_gettid = false;

    if (!lacks_gettid) {
      pid_t tid = syscall(__NR_gettid);
      if (tid != -1) {
        return (uint32_t)tid;
      }
      // Technically, this variable has to be volatile, but there is a small
      // performance penalty in accessing volatile variables and there should
      // not be any serious adverse effect if a thread does not immediately see
      // the value change to "true".
      lacks_gettid = true;
    }

    // If gettid() could not be used, we use one of the following.
    return (uint32_t)getpid(); 
  }

  // get the user id from system env
  std::string getUid() {
    if ( !std::getenv("USER") ) {
      //if no user available
      return std::string("null_user");
    }
    else
      return std::string(std::getenv("USER"));
  }

  // get the hostid from machine
  std::string getHostname() {
    if ( !std::getenv("HOSTNAME") ) {
        //if no host name available
      return std::string("null_host_name");
    }
    else
      return std::string(std::getenv("HOSTNAME"));
  }

  // receive one message, bytesize first
  void recv(::google::protobuf::Message &msg, 
      socket_ptr socket) 
  {
    try {
      int msg_size = 0;
  
      socket->receive(
          boost::asio::buffer(
            reinterpret_cast<char*>(&msg_size), 
            sizeof(int)), 0);
  
      if (msg_size<=0) {
        throw commError(
            "Invalid message size of " +
            std::to_string((long long)msg_size));
      }
      // TODO: mysterious issue, for receiving the acc request message
      //boost::this_thread::sleep_for(boost::chrono::microseconds(5)); 

      char* msg_data = new char[msg_size + 1];

      socket->receive(
          boost::asio::buffer(msg_data, msg_size), 0);
      msg_data[msg_size] = '\0';

      // DVLOG(2) << "recv msg(" << msg_size << "): " << msg_data;

      if (!msg.ParseFromArray(msg_data, msg_size)) {
        throw commError("Failed to parse input message");
      }
  
      delete [] msg_data;
    } catch (std::exception &e) {
      throw std::runtime_error(e.what());
    }
  }
  
  // send one message, bytesize first
  void send(::google::protobuf::Message &msg, 
      socket_ptr socket) 
  {
    try {
      int msg_size = msg.ByteSize();
  
      socket->send(
          boost::asio::buffer(
            reinterpret_cast<char*>(&msg_size), 
            sizeof(int)), 0);
  
      char* msg_data = new char[msg_size];

      msg.SerializeToArray(msg_data, msg_size);
  
      // DVLOG(2) << "send msg(" << msg_size << "): " << msg_data;

      socket->send(
          boost::asio::buffer(msg_data, msg_size),0);

    } catch (std::exception &e) {
      throw std::runtime_error(e.what());
    }
  }

  // 
  std::string readFile(std::string path) {
  
    std::ifstream fin(path, std::ios::binary);
  
    if (fin) {
      std::string data = std::string(
          std::istreambuf_iterator<char>(fin), 
          std::istreambuf_iterator<char>());
  
      return data;
    }
    else {
      throw fileError(path);
    }
  }

  std::string saveFile(
      std::string path, 
      const std::string &contents) 
  {
    if (boost::filesystem::native(path)) {
      throw std::runtime_error(std::string("Invalid path ")+path.c_str());
    }
    boost::filesystem::wpath file_path(path);
    boost::filesystem::wpath dir = file_path.parent_path();
    while (boost::filesystem::exists(file_path)) {
      std::string new_path = file_path.stem().string() + 
                             "_new" + 
                             file_path.extension().string();
      file_path = file_path.parent_path() / new_path;
    }
    boost::filesystem::create_directories(dir);
  
    FILE* fout = fopen(file_path.string().c_str(), "wb+");
    fwrite(contents.c_str(), sizeof(char), contents.length(), fout);
    fclose(fout);
  
    if (!boost::filesystem::exists(file_path)) {
      throw std::runtime_error(
          std::string("cannot write to ")+
          file_path.string());
    }
    return file_path.string();
  }
  
  bool deleteFile(std::string path) {
    boost::filesystem::wpath file(path);
    if (!boost::filesystem::exists(file)) {
      return false;
    }
    try {
      boost::filesystem::remove_all(file);
    } catch (boost::filesystem::filesystem_error &e) {
      DLOG(ERROR) << "Failed to delete file " << path
                  << " because: " << e.what();
      return false;
    }
    return true;
  }
}
