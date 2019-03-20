#include <stdio.h>
#include <sstream>
#include <iomanip>

#define LOG_HEADER "Client"
#include <glog/logging.h>

#include "blaze/Block.h"
#include "blaze/Client.h"

namespace blaze {

Client::Client(
    std::string _acc_id, 
    int _num_inputs, 
    int _num_outputs,
    int port):
  BaseClient(port, "127.0.0.1"),
  acc_id(_acc_id), 
  num_inputs(_num_inputs),
  num_outputs(_num_outputs),
  input_blocks_(_num_inputs),
  output_blocks_(_num_outputs),
  block_info_(_num_inputs, std::make_pair(0, false)),
  input_size_info_(_num_inputs),
  send_ack_(false)
{
  srand(time(NULL));

  // setup app_id
  std::stringstream ss;
  ss << "native-app-" << getTid();
  app_id = ss.str();
}

void* Client::createInput(
    int idx,
    int num_items, 
    int item_length, 
    int data_width, 
    int type) 
{

  if (idx >= num_inputs) {
    DLOG(ERROR) << "Index out of range: idx=" << idx
               << ", num_inputs=" << num_inputs;
    throw invalidParam("index out of range");
  }
  if (num_items <= 0 || 
      item_length <= 0 || 
      data_width <= 0 ||
      type > BLAZE_SHARED)
  {
    DLOG(ERROR) << num_items << ", "
               << item_length << ", "
               << data_width << ", "
               << type;
    throw invalidParam("Invalid input parameters");
  }
  
  // handle scalers
  if (num_items == 1 && item_length == 1) {
    if (data_width > 8) {
      LOG_IF(WARNING, VLOG_IS_ON(1)) << "Scalar input cannot be larger than 8 bytes, "
                   << "force it to be 8 bytes";
    }
    // since scalar variable uses a long long type in the message,
    // force data width to be 8 for safety
    data_width = 8; 
  }

  // record this in case the actual data size is smaller
  // than the DataBlock size
  input_size_info_[idx].num_items   = num_items;
  input_size_info_[idx].item_length = item_length;
  input_size_info_[idx].data_width  = data_width;

  // NOTE: allow user to overwrite input blocks
  if (input_blocks_[idx]) {
    DVLOG(1) << "block " << idx << " already allocated";

    if (input_blocks_[idx]->getSize() < num_items * item_length * data_width) {
      throw invalidParam("input block size too big");
    }
    return input_blocks_[idx]->getData();
  }
  else {
    // write data to memory mapped file
    // use thread id to create unique output file path
    // create a new block and add it to input table
    DataBlock_ptr block(new DataBlock(num_items, 
          item_length, item_length*data_width,
          0, // aligned width
          DataBlock::OWNED));

    input_blocks_[idx] = block;
    //DVLOG(1) << "Allocate block " << idx;

    // generate block info include (id, cached)
    int64_t block_id = ((int64_t)getTid()<<10) + idx;
    bool    cached   = false;

    if (type == BLAZE_INPUT_CACHED) {
      cached = true;
    }
    else if (type == BLAZE_SHARED) {
      block_id = 0 - block_id; // broadcast id is negative
      cached = true;
    }
    block_info_[idx] = std::make_pair(block_id, cached);

    return block->getData();
  }
}

void* Client::createOutput(
    int idx,
    int num_items, 
    int item_length, 
    int data_width)
{
  if (idx >= num_outputs) {
    throw invalidParam("Index out of range");
  }
  // NOTE: allow user to overwrite input blocks
  //
  // output block not ready, allocate it
  // check input variable
  if (item_length <= 0 || num_items <= 0 || data_width <= 0) {
    throw invalidParam("Invalid parameter(s)");
  }
  // create a new block and add it to output table
  DataBlock_ptr block(new DataBlock(num_items, 
        item_length, item_length*data_width, 0, 
        DataBlock::OWNED));

  output_blocks_[idx] = block;

  return (void*)block->getData();
}

// experimental feature
void Client::setInput(int idx, void* src,
    int num_items, 
    int item_length, 
    int data_width, 
    int type) 
{
  if (idx >= num_inputs) {
    throw invalidParam("Invalid input block");
  }
  if (num_items <= 0 || 
      item_length <= 0 || 
      data_width <= 0 ||
      type >= BLAZE_SHARED)
  {
    throw invalidParam("Invalid input parameters");
  }
  void *dst = createInput(idx, num_items, item_length, data_width, type);
  memcpy(dst, src, num_items*item_length*data_width);
}

void* Client::getInputPtr(int idx) {
  if (idx >= num_inputs || !input_blocks_[idx]) {
    throw invalidParam("Invalid input block");
  }
  return input_blocks_[idx]->getData();
}

int Client::getInputNumItems(int idx) {
  if (idx >= num_inputs || !input_blocks_[idx]) {
    throw invalidParam("Invalid input index");
  }
  return input_blocks_[idx]->getNumItems();
}

int Client::getInputLength(int idx) {
  if (idx >= num_inputs || !input_blocks_[idx]) {
    throw invalidParam("Invalid input index");
  }
  return input_blocks_[idx]->getLength();
}

void* Client::getOutputPtr(int idx) {
  if (idx >= num_outputs || !output_blocks_[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks_[idx]->getData();
}

int Client::getOutputNumItems(int idx) {
  if (idx >= num_outputs || !output_blocks_[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks_[idx]->getNumItems();
}

int Client::getOutputLength(int idx) {
  if (idx >= num_outputs || !output_blocks_[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks_[idx]->getLength();
}

void Client::start(bool blocking) {
  PLACE_TIMER;

  try {
    // send request
    TaskMsg request_msg;
    TaskMsg reply_msg;
    {
      PLACE_TIMER1("prepareRequest");
      prepareRequest(request_msg);
    }
    {
      PLACE_TIMER1("send and receive reply");
      send(request_msg);

      VLOG(2) << "Sent a request";

      // wait on reply for ACCREQUEST
      recv(reply_msg);
    }

    if (reply_msg.type() == ACCGRANT) {
      PLACE_TIMER1("prepareData");

      TaskMsg data_msg;
      prepareData(data_msg, reply_msg);
      send(data_msg);
      VLOG(2) << "Sent data";
    }
    else {
      throw std::runtime_error("request rejected");
    }

    TaskMsg finish_msg;
    {
      PLACE_TIMER1("wait finish_msg");
      // wait on reply for ACCDATA
      recv(finish_msg);
    }

    if (finish_msg.type() == ACCFINISH) {
      PLACE_TIMER1("processOutput");
      processOutput(finish_msg);
    }
    else {
      DLOG(ERROR) << "Received " << finish_msg.type() 
        << " instead of ACCFINISH";
      throw std::runtime_error("did not receive ACCFINISH");
    }
    
    // reply an ACCFINISH if one or more output blocks
    // are owned by NAM
    if (send_ack_) {
      TaskMsg m;
      m.set_type(MsgType::ACCFINISH);
      try {
        send(m);
        RVLOG(INFO, 1) << "Sent ACK to NAM";
      } catch (std::exception &e) {
        RVLOG(ERROR, 1) << "Fail to send ACK to NAM";
        // do nothing if we cannot send an ACK message
      }
    }
  }
  catch (std::exception &e) {
    VLOG(1) << "Task failed";
    VLOG(1) << "Perform computation on CPU";
    
    compute();
  }
  reset();
}

void Client::prepareRequest(TaskMsg &msg) {
  PLACE_TIMER;

  msg.set_type(ACCREQUEST);
  msg.set_acc_id(acc_id);
  msg.set_app_id(app_id);

  for (int i = 0; i < num_inputs; i++) {
    DataMsg *data_msg = msg.add_data();
    
    // check if the data is scalar
    if (input_blocks_[i]->getNumItems() == 1 && 
        input_blocks_[i]->getItemLength() == 1)
    {
      char* data = (char*)input_blocks_[i]->getData();
      data_msg->set_scalar_value(*((long long*)data));
      VLOG(2) << "Sent scalar input " << i;
    }
    else {
      data_msg->set_partition_id(block_info_[i].first);
      if (!block_info_[i].second) {
        data_msg->set_cached(false);
      }
    }
  }
  VLOG(1) << "Requesting accelerator " << acc_id;
}

void Client::prepareData(TaskMsg &accdata_msg, TaskMsg &reply_msg) {
 
  VLOG(1) << "Start writing data to memory";
  accdata_msg.set_type(ACCDATA);

  int blockIdx = 0;  // index to input_blocks_
  for (int i = 0; i < reply_msg.data_size(); i++) {
    // skip scalar input blocks
    while (input_blocks_[blockIdx]->getNumItems() == 1 && 
           input_blocks_[blockIdx]->getItemLength() == 1)
    {
      blockIdx ++; 
    }
    if (!reply_msg.data(i).cached()) {
      PLACE_TIMER1(std::string("input block ") + std::to_string((uint64_t)i));

      DataMsg *data_msg = accdata_msg.add_data();
      data_msg->set_partition_id(block_info_[blockIdx].first);

      DataBlock_ptr block = input_blocks_[blockIdx];
      BlockSizeInfo info = input_size_info_[blockIdx];

      data_msg->set_file_path(block->get_path());
      //data_msg->set_num_elements(block->getNumItems());
      //data_msg->set_element_length(block->getItemLength());
      //data_msg->set_element_size(block->getItemSize());
      data_msg->set_num_elements(info.num_items);
      data_msg->set_element_length(info.item_length);
      data_msg->set_element_size(info.data_width * info.item_length);
      
      VLOG(1) << "Finish writing " << i;
    }
    blockIdx ++;
  }
}

void Client::processOutput(TaskMsg &msg) {

  PLACE_TIMER;
  VLOG(1) << "Task finished, start reading output";

  if (num_outputs != msg.data_size()) {
    DLOG(ERROR) << "Exprected #output=" << num_outputs 
               << ", received #output=" << msg.data_size();
    throw commError("Num of output blocks mismatch");
  }

  for (int i = 0; i < msg.data_size(); i++) {
    DataMsg data_msg = msg.data(i);

    // check data_msg fields
    if (!data_msg.has_num_elements() ||
        !data_msg.has_element_length() ||
        !data_msg.has_element_size() ||
        !data_msg.has_file_path())
    {
      throw commError("Missing field in ACCDATA");
    }

    // create block 
    int num_items    = data_msg.num_elements();	
    int item_length  = data_msg.element_length();	
    int item_size    = data_msg.element_size();	

    std::string path = data_msg.file_path();
    VLOG(1) << "Reading output from " << path;

    if (data_msg.has_cached() && data_msg.cached()) {
      // if the block is cached, that means it is owned
      // by NAM, so we don't delete it 
      DataBlock_ptr block(new DataBlock(
            path, num_items, item_length, item_size, 
            0, DataBlock::SHARED));

      output_blocks_[i] = block;

      send_ack_ = true;
    }
    else {
      DataBlock_ptr block(new DataBlock(
            path, num_items, item_length, item_size, 
            0, DataBlock::OWNED));

      output_blocks_[i] = block;
    }
  }
  VLOG(1) << "Finish reading output";
}
} // namespace blaze
