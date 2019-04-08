#include <boost/filesystem.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/smart_ptr.hpp>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <ifaddrs.h>
#include <netinet/in.h> 
//#include <boost/md5.hpp>

#ifdef NDEBUG
#define LOG_HEADER "Block"
#endif
#include <glog/logging.h>

#include "blaze/Block.h"

namespace blaze {

static inline std::string get_block_path() {
  std::stringstream ss;
  ss << local_dir << "/"
     << "blaze-block-"
     << getTid() << "-"
     << getTS() << ".dat";
  return ss.str();
}

void DataBlock::calc_sizes(
    int _num_items, 
    int _item_length,
    int _item_size,
    int _align_width) 
{
  data_width = _item_size / _item_length;

  if (_align_width == 0 ||
      _item_size % _align_width == 0) 
  {
    item_size = _item_size;
    is_aligned_ = false;
  }
  else {
    item_size = (_item_length*data_width + _align_width - 1) /
      _align_width * _align_width;
    is_aligned_ = true;
  }
  length = num_items * item_length;
  size_  = num_items * item_size;

  if (length <= 0 || size_ <= 0 || data_width < 1) {
    throw std::runtime_error("Invalid parameters");
  }
  used_size_ = size_;
}

void DataBlock::map_region() {
  PLACE_TIMER;
  using namespace boost::interprocess;

  boost::shared_ptr<file_mapping> m_file(new file_mapping(
        mm_file_path_.c_str(), read_write));

  boost::shared_ptr<mapped_region> m_region(new mapped_region(
        *m_file, read_write, 0, size_));

  mm_region_ = m_region;
  mm_file_   = m_file;
}

void DataBlock::updateBlockInfo(int _num_items, int _item_length, int _item_size ){
    num_items = _num_items;
    item_length = _item_length;
    item_size = _item_size;
    //setUsedSize(_item_size);
    used_size_ = _num_items * _item_size;
}
// create basic data block from path
DataBlock::DataBlock(
    std::string path,
    int _num_items, 
    int _item_length,
    int _item_size,
    int _align_width,
    std::string _ip_address,
    int _port, // port to connect
    Flag _flag,
    ConfigTable_ptr _conf):
  num_items(_num_items),
  item_length(_item_length),
  align_width(_align_width),
  flag_(_flag),
  mm_file_path_(path),
  ip_address(_ip_address), //
  srv_port(_port), // 
  conf_(_conf)
{
  PLACE_TIMER1("constrct 1");
  calc_sizes(_num_items, _item_length, _item_size, _align_width);
   
  #if 0

  if (path.empty() || !file_exists(mm_file_path_)) {
    DLOG(ERROR) << "block path: " << path << " is invalid";
    throw fileError(__func__);
  }
  map_region();
  is_ready_ = true;
#else

  mm_file_path_ = get_block_path();
  fprintf(stderr, "@@@@pp: Block.cpp, new SHARED Block start!!\n");
  if (file_exists(mm_file_path_)) {
    DLOG(ERROR) << "block path "<< mm_file_path_ << " already exists";
    throw fileError(__func__ + std::string(" failed"));
  }
  else {
    PLACE_TIMER1("allocate mmap region");
    std::filebuf fbuf;
    fbuf.open(mm_file_path_, 
        std::ios_base::in | std::ios_base::out | 
        std::ios_base::trunc | std::ios_base::binary);

    // set the size
    fbuf.pubseekoff(size_-1, std::ios_base::beg);
    fbuf.sputc(0);

    VLOG(1) << "Allocate block at " << mm_file_path_;
  }

  map_region();

  fprintf(stderr, "@@@@pp: Block.cpp !!, mm_file_path_:%s, %d, %d, %d\n", mm_file_path_.c_str(), _num_items, _item_length, _item_size);

  is_ready_ = true;
  fprintf(stderr, "@@@@pp: Block.cpp, new SHARED Block end!!\n");
#endif
  fprintf(stderr, "@@@@pp: Block.cpp, SHARED, port: %d\n", srv_port);
  //std::string _address="127.0.0.1";
  //std::string _address="10.0.0.33";
  std::string _address = ip_address;
  ios_ptr      _ios(new boost::asio::io_service);
  endpoint_ptr _endpoint(new boost::asio::ip::tcp::endpoint(
              boost::asio::ip::address::from_string(_address), 
              srv_port));


  ios = _ios;
  endpoint = _endpoint;

  try { 
      char * baseline = (char*) getData();
      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 0!\n");
      socket_ptr sock(new boost::asio::ip::tcp::socket(*ios));

      sock->connect(*endpoint);
      sock->set_option(boost::asio::ip::tcp::no_delay(true));
      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 1!\n");

      int msg_size = 0;

      sock->receive(
              boost::asio::buffer(
                  reinterpret_cast<char*>(&msg_size), 
                  sizeof(int)), 0);

      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 2! should receive 101: actual: %d\n", msg_size);
      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 3! should receive size: %d\n", getUsedSize());
      //sock_ = sock;
      //connected_ = true;
#if 1

      // turn off Nagle Algorithm to improve latency
      sock->set_option(boost::asio::ip::tcp::no_delay(true));
      boost::asio::socket_base::receive_buffer_size option0;
      sock->get_option(option0);
      int option0_size = option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, SHARED: option0_size: %d \n", option0_size);

      // set socket buffer size to be 4MB
      boost::asio::socket_base::receive_buffer_size option(4*1024*1024);
      sock->set_option(option); 
      sock->get_option(option0);
      option0_size = option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, SHARED: option0_size: %d \n", option0_size);
      //char * msg_data = new char[getUsedSize()];
      char * msg_data = (char*) getData();
      fprintf(stderr, "check before: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
      //sock->receive(
      //        boost::asio::buffer(
      //            reinterpret_cast<char*>(getData()),
      //            //msg_data, 
      //            getUsedSize()),
      //        0);
      boost::system::error_code error;
      int bytes_transferred = 0;
      bytes_transferred = 
                  //boost::asio::read(sock.get(),
                  boost::asio::read(*(sock.get()),
                  //sock.get(), 
                  boost::asio::buffer(
                      reinterpret_cast<char*>(getData()),getUsedSize()));
      //            boost::asio::transfer_exactly(getUsedSize()));
      //            &error);
      //if( error && error != boost::asio::error::eof ) {
      //    fprintf(stderr, "receive failed: %s\n" , error.message().c_str());
      //}
      //else {
      //    fprintf(stderr, "receive successfully bytes_transferred: %d\n", bytes_transferred);
      //}

      fprintf(stderr, "read receive successfully bytes_transferred: %d\n", bytes_transferred);


      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 4! finished receive\n");
      fprintf(stderr, "check after: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
  fprintf(stderr, "checkData 2: 0:%d, -1: %d, -2:%d, -3:%d\n", msg_data[getUsedSize()-1], msg_data[getUsedSize()-2], msg_data[getUsedSize()-3], msg_data[getUsedSize()-4]);
  fprintf(stderr, "checkData 2 half: 0:%d, -1: %d, -2:%d, -3:%d\n", msg_data[getUsedSize()/2-1], msg_data[getUsedSize()/2-2], msg_data[getUsedSize()/2-3], msg_data[getUsedSize()/2-4]);
#else
      char * msg_data = new char[getUsedSize()];
      fprintf(stderr, "check before msg_data: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
      fprintf(stderr, "check before baseline: 0:%d, 1: %d, 2:%d, 3:%d\n", baseline[0], baseline[1], baseline[2], baseline[3]);
      sock->receive(
              boost::asio::buffer(
                  //reinterpret_cast<char*>(getData()),
                  msg_data, 
                  getUsedSize()),
              0);
      fprintf(stderr, "@@@@pp: Block.cpp, receiver part 4! finished receive\n");
      fprintf(stderr, "check after  msg_data: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
      fprintf(stderr, "check after  baseline: 0:%d, 1: %d, 2:%d, 3:%d\n", baseline[0], baseline[1], baseline[2], baseline[3]);
      
      fprintf(stderr, "@@@@pp: Block.cpp, recvPath: %s\n", mm_file_path_.c_str());  
      int errCount = 0;
      fprintf(stderr, "@@@@p: Block.cpp, start check\n");
      for(int i = 0; i < getUsedSize(); i++){
          if(msg_data[i]!= baseline[i]){
              fprintf(stderr, "@@@@p: Block.cpp, checkShared: pos: %d, baseline: %d, msg_data: %d\n", i, baseline[i], msg_data[i]);
              errCount++;
          }    
      }
      fprintf(stderr, "@@@@p: Block.cpp, errCount/total: %d/%d\n", errCount, getUsedSize());

#endif
      //memcpy(getData(), msg_data, getUsedSize());


  }
  catch (const boost::system::system_error &e) {
      DLOG(ERROR) << "Cannot establish communication with "<<ip_address<<" port:" << srv_port;
  } 


}

// allocate a block with a pre-allocated pth
DataBlock::DataBlock(
    int _num_items, 
    int _item_length,
    int _item_size,
    int _align_width,
    Flag _flag,
    ConfigTable_ptr _conf):
  num_items(_num_items),
  item_length(_item_length),
  align_width(_align_width),
  flag_(_flag),
  conf_(_conf)
{
  PLACE_TIMER1("constrct 2");
  calc_sizes(_num_items, _item_length, _item_size, _align_width);

  // create a memory mapped region
  mm_file_path_ = get_block_path();

  if (file_exists(mm_file_path_)) {
    DLOG(ERROR) << "block path "<< mm_file_path_ << " already exists";
    throw fileError(__func__ + std::string(" failed"));
  }
  else {
    PLACE_TIMER1("allocate mmap region");
    std::filebuf fbuf;
    fbuf.open(mm_file_path_, 
        std::ios_base::in | std::ios_base::out | 
        std::ios_base::trunc | std::ios_base::binary);

    // set the size
    fbuf.pubseekoff(size_-1, std::ios_base::beg);
    fbuf.sputc(0);

    VLOG(1) << "Allocate block at " << mm_file_path_;
  }

    VLOG(1) << "@@pp: Block.cpp: before map_region(), Allocate block at " << mm_file_path_;
  map_region();
    VLOG(1) << "@@pp: Block.cpp: after  map_region(), Allocate block at " << mm_file_path_<<", num_iterms: "<<num_items<<", item_length: "<<item_length<<", item_size: "<<_item_size;

    fprintf(stderr, "@@@@pp: Block.cpp !!, mm_file_path_:%s, %d, %d, %d\n", mm_file_path_.c_str(), _num_items, _item_length, _item_size);
    //fflush(stdout);
  // setup the flags
  if (_flag == SHARED) {
    // block is to share with client/host
    is_ready_ = true;
  }
  else { // flag == OWNED
    // block is a scratch, for client/host to write
    // but will be deleted by owner
    is_ready_ = false;
  }

    // peipei added:
    srv_port = 0;
    fprintf(stderr,"@@@@pp: Block.cpp, create the ios service and port listening part\n");

    //std::string _address="127.0.0.1";
    std::string _address="127.0.0.1";
  struct ifaddrs* ifAddrStruct = NULL;
  getifaddrs(&ifAddrStruct);
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
      if(ip_addr.find("10.0.") != std::string::npos){
          fprintf(stderr, "@@@@pp: Block.cpp, ip_addr here is %s\n", ip_addr.c_str());
          _address = ip_addr.c_str();
      }
      fprintf(stderr, "@@@@pp: Block.cpp, ip_addr has %s\n", ip_addr.c_str());
    }
  }

    //std::string _address="10.0.0.33";
    ios_ptr      _ios(new boost::asio::io_service);
    endpoint_ptr _endpoint(new boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string(_address), 
                0));
    acceptor_ptr _acceptor(new boost::asio::ip::tcp::acceptor(*_ios, *_endpoint));
    
    ios = _ios;
    endpoint = _endpoint;
    acceptor = _acceptor;
    //_acceptor->listen();
    srv_port=_acceptor->local_endpoint().port();
    // start io service processing loop
    boost::asio::io_service::work work(*ios);

    //DLOG(ERROR) << "@@@@pp: _max_threads : "<<_max_threads;

    for (int t=0; t<1; t++) 
    {
        comm_threads.create_thread(
                boost::bind(&boost::asio::io_service::run, ios.get()));
    }

    // asynchronously start listening for new connections
    startAccept();
    fprintf(stderr,"@@@@pp: 2, Block.cpp !!,Listening for new connections at %s: %d\n",
    _address.c_str(), srv_port);

}


void DataBlock::startAccept() {
  socket_ptr sock(new boost::asio::ip::tcp::socket(*ios));
  acceptor->async_accept(*sock,
      boost::bind(&DataBlock::handleAccept,
                  this,
                  boost::asio::placeholders::error,
                  sock));
}

void DataBlock::handleAccept(
    const boost::system::error_code& error,
    socket_ptr sock) 
{
  if (!error) {
    ios->post(boost::bind(&DataBlock::process, this, sock));
    startAccept();
  }
}

void DataBlock::process(socket_ptr sock) {

  char * msg_data = (char*) debugGetData();
  fprintf(stderr, "start to process 0!\n");
  fprintf(stderr, "checkData 0: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);

  // turn off Nagle Algorithm to improve latency
  sock->set_option(boost::asio::ip::tcp::no_delay(true));

      // turn off Nagle Algorithm to improve latency
      boost::asio::socket_base::receive_buffer_size receive_option0;
      sock->get_option(receive_option0);
      int receive_option0_size = receive_option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, receive_option0_size 0: %d \n", receive_option0_size);

      // set socket buffer size to be 4MB
      boost::asio::socket_base::receive_buffer_size option(4*1024*1024);
      sock->set_option(option); 

      sock->get_option(receive_option0);
      receive_option0_size = receive_option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, receive_option0_size 1: %d \n", receive_option0_size);


      // turn off Nagle Algorithm to improve latency
      boost::asio::socket_base::send_buffer_size send_option0;
      sock->get_option(send_option0);
      int send_option0_size = send_option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, send_option0_size 0: %d \n", send_option0_size);

      // set socket buffer size to be 4MB
      boost::asio::socket_base::send_buffer_size send_option(4*1024*1024);
      sock->set_option(send_option); 

      sock->get_option(send_option0);
      send_option0_size = send_option0.value();

      fprintf(stderr,"@@@@pp: Block.cpp, send_option0_size 1: %d \n", send_option0_size);


  

  int msg_size = 101;
  fprintf(stderr, "@@@@pp: Block.cpp, send part 0!\n");

  sock->send(
          boost::asio::buffer(
              reinterpret_cast<char*>(&msg_size), 
              sizeof(int)), 0);
  fprintf(stderr, "@@@@pp: Block.cpp, send part 1!\n");
  fprintf(stderr, "@@@@pp: Block.cpp, send part 2!  should send size: %d\n", getUsedSize());
  fprintf(stderr, "checkData 1: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
  //sock->send(
  //        boost::asio::buffer(
  //            reinterpret_cast<char*>(debugGetData()),
  //            getUsedSize()),
  //            0);

      int bytes_transferred = 0;
      bytes_transferred = 
  boost::asio::write(*(sock.get()),
          boost::asio::buffer(
              reinterpret_cast<char*>(debugGetData()),
              getUsedSize()));
              //0);
      fprintf(stderr, "write successfully bytes_transferred: %d\n", bytes_transferred);

  fprintf(stderr, "@@@@pp: Block.cpp, sendPath:%s\n", mm_file_path_.c_str());
  fprintf(stderr, "@@@@pp: Block.cpp, send part 3! finish send\n");
  fprintf(stderr, "checkData 2: 0:%d, 1: %d, 2:%d, 3:%d\n", msg_data[0], msg_data[1], msg_data[2], msg_data[3]);
  fprintf(stderr, "checkData 2: 0:%d, -1: %d, -2:%d, -3:%d\n", msg_data[getUsedSize()-1], msg_data[getUsedSize()-2], msg_data[getUsedSize()-3], msg_data[getUsedSize()-4]);
  fprintf(stderr, "checkData 2 half: 0:%d, -1: %d, -2:%d, -3:%d\n", msg_data[getUsedSize()/2-1], msg_data[getUsedSize()/2-2], msg_data[getUsedSize()/2-3], msg_data[getUsedSize()/2-4]);
  //std::string md5value = boost::md5((void*) msg_data, getUsedSize());
  //fprintf(stderr, "@@@@pp: Block.cpp, send md5: %s, size: %n", md5value.hex_str_value().c_str(), getUsedSize());

}

DataBlock::DataBlock(const DataBlock &block) {

  DVLOG(2) << "Create a duplication of a block";

  num_items = block.num_items;
  item_length = block.item_length;
  item_size = block.item_length;
  data_width = block.data_width;
  align_width = block.align_width;
  length = block.length;

  flag_ = SHARED;
  size_ = block.size_;

  is_aligned_ = block.is_aligned_;
  is_ready_  = block.is_ready_;

  mm_region_ = block.mm_region_;
  //peipei added:
  //srv_port = block.srv_port;  
}

DataBlock::~DataBlock() {
  if (mm_region_ && flag_ == OWNED) {
    boost::interprocess::file_mapping::remove(mm_file_path_.c_str());
    boost::filesystem::remove(boost::filesystem::path(mm_file_path_));
    DVLOG(1) << "Release block at " << mm_file_path_;
    fprintf(stderr, "Release OWNED block %s: %d, %d, %d\n", mm_file_path_.c_str(), num_items, item_length, item_size);
    
    //peipei added
 ios->stop();
  comm_threads.interrupt_all();
  comm_threads.join_all();
  fprintf(stderr,"Block.cpp, DataBlock %s stopped listening\n", mm_file_path_.c_str());
  }
#if 1
  if (mm_region_ && flag_ == SHARED){
    boost::interprocess::file_mapping::remove(mm_file_path_.c_str());
    boost::filesystem::remove(boost::filesystem::path(mm_file_path_));
    DVLOG(1) << "Release block at " << mm_file_path_;
    fprintf(stderr, "Release SHARED block %s: %d, %d, %d\n", mm_file_path_.c_str(), num_items, item_length, item_size);
  }
#endif
  // mm_region_ should be released by itself

}

std::string DataBlock::get_path() {
  return mm_file_path_;
}

// Deprecated
#if 0
void DataBlock::alloc() {
  if (!mm_region_ && size_ > 0) {

    if (!mm_file_path_.empty()) {
      using namespace boost::interprocess;
      // create a mmap file 
      boost::shared_ptr<file_mapping> m_file(
          new file_mapping(mm_file_path_, read_write));

      if (!m_file) {
        DLOG(INFO) << "failed to map mmfile " << mm_file_path;
        throw internalError("DataBlock::alloc() error");
      }

      mm_file_ = m_file;

      // allocate the mmfile region
      boost::shared_ptr<bi::mapped_region> region(
          new mapped_region(*mm_file, read_write, 0, size_));

      mm_region_ = region;
    }
    else {
      DLOG(INFO) << "cannot allocate before file path is set";
      throw internalError("DataBlock::alloc() error");
    }
  }
}
#endif

void* DataBlock::debugGetData(){
  return mm_region_->get_address(); 
}
void* DataBlock::getData() { 
  return mm_region_->get_address(); 
}

void DataBlock::setReady() {
  if (flag_ == OWNED) {
    is_ready_ = true;
  }
}

bool DataBlock::isAllocated() { 
  return true;
}

bool DataBlock::isReady() { 
  return is_ready_; 
}

void DataBlock::writeData(void* src, size_t _size) {
  if (_size > size_) {
    throw std::runtime_error("Not enough space left in Block");
  }
  PLACE_TIMER;
  if (!is_aligned_) {
    writeData(src, _size, 0);
  }
  else {
    for (int k=0; k<num_items; k++) {
      int data_size = item_length*data_width;
      writeData((void*)((char*)src + k*data_size), 
          data_size, k*item_size);
    }  
    is_ready_ = true;
  }
}

// copy data from an array with offset
// this method needs to be locked from outside
void DataBlock::writeData(
    void* src, 
    size_t _size, 
    size_t _offset)
{
  if (_offset+_size > size_) {
    throw std::runtime_error("exceeds block size");
  }
  PLACE_TIMER;

  memcpy((char*)mm_region_->get_address()+_offset, src, _size);

  if (_offset + _size == size_) {
    is_ready_ = true;
  }
}

// write data to an array
void DataBlock::readData(void* dst, size_t size) {
  PLACE_TIMER;
  memcpy(dst, mm_region_->get_address(), size);
}

// Deprecated
DataBlock_ptr DataBlock::sample(char* mask) {

#if 1
  throw internalError("Block::sample is deprecated");
#else
  // count the total number of 
  int masked_items = 0;
  for (int i=0; i<num_items; i++) {
    if (mask[i]!=0) {
      masked_items ++;
    }
  }
  
  DataBlock_ptr block(new DataBlock(
        masked_items,
        item_length, 
        item_size,
        aligned ? align_width : item_size));

  char* masked_data = block->getData();

  int k=0;
  for (int i=0; i<num_items; i++) {
    if (mask[i] != 0) {
      memcpy(masked_data+k*item_size, 
             mm_region_->get_address()+i*item_size, 
             item_size);
      k++;
    }
  }
  block->ready = true;

  return block;
#endif
}

void DataBlock::readFromMem(std::string path) {
  is_ready_ = true;
  DVLOG(2) << "Calling a dummy function";
}

void DataBlock::writeToMem(std::string path) {
  DVLOG(2) << "Calling a dummy function";
}

} // namespace
