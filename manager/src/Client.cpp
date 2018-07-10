#include <stdio.h>
#include <sstream>
#include <iomanip>

//#define LOG_HEADER "Client"
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
  input_blocks(_num_inputs, NULL_DATA_BLOCK),
  output_blocks(_num_outputs, NULL_DATA_BLOCK),
  block_info(_num_inputs, std::make_pair(0, false))
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
  else if (input_blocks[idx]) {
    LOG_IF(WARNING, VLOG_IS_ON(1)) << "Block with idx=" << idx
                 << " is already allocated, returning data ptr";
    return (void*)input_blocks[idx]->getData();
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

  if (num_items == 1 && item_length == 1) {
    if (data_width > 8) {
      LOG_IF(WARNING, VLOG_IS_ON(1)) << "Scalar input cannot be larger than 8 bytes, "
                   << "force it to be 8 bytes";
    }
    // since scalar variable uses a long long type in the message,
    // force data width to be 8 for safety
    data_width = 8; 
  }

  // create a new block and add it to input table
  DataBlock_ptr block(new DataBlock(num_items, 
        item_length, item_length*data_width));

  input_blocks[idx] = block;

  // generate block info include (id, cached)
  int64_t block_id = ((int64_t)getTid()<<10) + idx;
  bool    cached   = false;

  if (type==BLAZE_INPUT_CACHED) {
    cached = true;
  }
  else if (type==BLAZE_SHARED) {
    block_id = 0 - block_id; // broadcast id is negative
    cached = true;
  }
  block_info[idx] = std::make_pair(block_id, cached);

  return block->getData();
}


void* Client::createInput(
    int idx,
    int num_items, 
    int item_length, 
    int data_width, 
    std::pair<std::string, int>& ext_flag, 
    int type)
{
  if (idx >= num_inputs) {
    DLOG(ERROR) << "Index out of range: idx=" << idx
               << ", num_inputs=" << num_inputs;
    throw invalidParam("index out of range");
  }
  else if (input_blocks[idx]) {
    LOG_IF(WARNING, VLOG_IS_ON(1)) << "Block with idx=" << idx
                 << " is already allocated, returning data ptr";
    return (void*)input_blocks[idx]->getData();
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

  if (num_items == 1 && item_length == 1) {
    if (data_width > 8) {
      LOG_IF(WARNING, VLOG_IS_ON(1)) << "Scalar input cannot be larger than 8 bytes, "
                   << "force it to be 8 bytes";
    }
    // since scalar variable uses a long long type in the message,
    // force data width to be 8 for safety
    data_width = 8; 
  }

  // create a new block and add it to input table
  DataBlock_ptr block(new DataBlock(num_items, 
        item_length, item_length*data_width, ext_flag));

  input_blocks[idx] = block;

  // generate block info include (id, cached)
  int64_t block_id = ((int64_t)getTid()<<10) + idx;
  bool    cached   = false;

  if (type==BLAZE_INPUT_CACHED) {
    cached = true;
  }
  else if (type==BLAZE_SHARED) {
    block_id = 0 - block_id; // broadcast id is negative
    cached = true;
  }
  block_info[idx] = std::make_pair(block_id, cached);

  return block->getData();
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
  else if (!output_blocks[idx]) {
    // output block not ready, allocate it
    // check input variable
    if (item_length<=0 || num_items<=0 || data_width<=0) {
      throw invalidParam("Invalid parameter(s)");
    }
    // create a new block and add it to output table
    DataBlock_ptr block(new DataBlock(num_items, 
          item_length, item_length*data_width));

    output_blocks[idx] = block;
    
    return (void*)block->getData();
  }
  else {
    DataBlock_ptr block = output_blocks[idx];
    if (num_items != block->getNumItems() ||
        item_length != block->getItemLength() ||
        data_width != block->getItemSize()/block->getItemLength())
    {
      LOG_IF(WARNING, VLOG_IS_ON(1)) << "Output " << idx << "is already created, "
                   << "but the original dimensions do not match the provided ones."
                   << " Using original dimensions.";
    }
    return block->getData();
  }
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
  if (!input_blocks[idx]) {
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
  else {
    DataBlock_ptr block = input_blocks[idx];
    if (num_items !=  block->getNumItems() ||
        item_length != block->getItemLength() ||
        item_length*data_width != block->getItemSize()) 
    {
      throw invalidParam("Block size does not match");
    }
    memcpy((void*)block->getData(), src, 
        input_blocks[idx]->getSize());
  }
}


// experimental feature
void Client::setInput(int idx, void* src,
    std::pair<std::string, int>& ext_flag,
    int num_items, 
    int item_length, 
    int data_width, 
    int type)
{
  if (idx >= num_inputs) {
    throw invalidParam("Invalid input block");
  }
  if (!input_blocks[idx]) {
    if (num_items <= 0 || 
        item_length <= 0 || 
        data_width <= 0 ||
        type >= BLAZE_SHARED)
    {
      throw invalidParam("Invalid input parameters");
    }
    void *dst = createInput(idx, num_items, item_length, data_width, ext_flag, type);
    memcpy(dst, src, num_items*item_length*data_width);
  }
  else {
    DataBlock_ptr block = input_blocks[idx];
    if (num_items !=  block->getNumItems() ||
        item_length != block->getItemLength() ||
        item_length*data_width != block->getItemSize()) 
    {
      throw invalidParam("Block size does not match");
    }
    memcpy((void*)block->getData(), src, 
        input_blocks[idx]->getSize());
  }
}

void* Client::getInputPtr(int idx) {
  if (idx >= num_inputs || !input_blocks[idx]) {
    throw invalidParam("Invalid input block");
  }
  return input_blocks[idx]->getData();
}

int Client::getInputNumItems(int idx) {
  if (idx >= num_inputs || !input_blocks[idx]) {
    throw invalidParam("Invalid input index");
  }
  return input_blocks[idx]->getNumItems();
}

int Client::getInputLength(int idx) {
  if (idx >= num_inputs || !input_blocks[idx]) {
    throw invalidParam("Invalid input index");
  }
  return input_blocks[idx]->getLength();
}

void* Client::getOutputPtr(int idx) {
  if (idx >= num_outputs || !output_blocks[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks[idx]->getData();
}

int Client::getOutputNumItems(int idx) {
  if (idx >= num_outputs || !output_blocks[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks[idx]->getNumItems();
}

int Client::getOutputLength(int idx) {
  if (idx >= num_outputs || !output_blocks[idx]) {
    throw invalidParam("Output not ready or index out of range");
  }
  return output_blocks[idx]->getLength();
}

void Client::start(bool blocking) {

  try {
    // send request
    TaskMsg request_msg;
    prepareRequest(request_msg);
    send(request_msg);

    VLOG(2) << "Sent a request";

    // wait on reply for ACCREQUEST
    TaskMsg reply_msg;
    recv(reply_msg);

    if (reply_msg.type() == ACCGRANT) {

      TaskMsg data_msg;
      prepareData(data_msg, reply_msg);
      send(data_msg);
      VLOG(2) << "Sent data";
    }
    else {
      throw std::runtime_error("request rejected");
    }

    TaskMsg finish_msg;
    // wait on reply for ACCDATA
    recv(finish_msg);

    if (finish_msg.type() == ACCFINISH) {
      processOutput(finish_msg);
    }
    else {
      DLOG(ERROR) << "Received " << finish_msg.type() 
        << " instead of ACCFINISH";
      throw std::runtime_error("did not receive ACCFINISH");
    }
     
  }
  catch (std::exception &e) {
    VLOG(1) << "Task failed because: " << e.what();
    VLOG(1) << "Perform computation on CPU";
    
    compute();
  }
}

void Client::prepareRequest(TaskMsg &msg) {

  msg.set_type(ACCREQUEST);
  msg.set_acc_id(acc_id);
  msg.set_app_id(app_id);

  for (int i=0; i<num_inputs; i++) {
    DataMsg *data_msg = msg.add_data();
    
    // check if the data is scalar
    if (input_blocks[i]->getNumItems() == 1 && 
        input_blocks[i]->getItemLength() == 1)
    {
      char* data = input_blocks[i]->getData();
      data_msg->set_scalar_value(*((long long*)data));
      VLOG(2) << "Sent scalar input " << i;
    }
    else {
      data_msg->set_partition_id(block_info[i].first);
      if (!block_info[i].second) {
        data_msg->set_cached(false);
      }
    }
  }
  VLOG(1) << "Requesting accelerator " << acc_id;
}

void Client::prepareData(TaskMsg &accdata_msg, TaskMsg &reply_msg) {
 
  VLOG(1) << "Start writing data to memory";
  accdata_msg.set_type(ACCDATA);

  int blockIdx = 0;  // index to input_blocks
  for (int i=0; i<reply_msg.data_size(); i++) {
    // skip scalar input blocks
    while (input_blocks[blockIdx]->getNumItems() == 1 && 
           input_blocks[blockIdx]->getItemLength() == 1)
    {
      blockIdx ++; 
    }
    if (!reply_msg.data(i).cached()) {

      DataMsg *data_msg = accdata_msg.add_data();
      data_msg->set_partition_id(block_info[blockIdx].first);

      // write data to memory mapped file
      // use thread id to create unique output file path
      std::stringstream path_stream;
      std::string output_dir = "/tmp";

      path_stream << output_dir << "/"
        << "client-data-"
        << getTid() 
        << std::setw(12) << std::setfill('0') << rand() 
        << i
        << ".dat";

      std::string path = path_stream.str();

      DataBlock_ptr block = input_blocks[blockIdx];
      block->writeToMem(path);

      data_msg->set_file_path(path);
      data_msg->set_num_elements(block->getNumItems());
      data_msg->set_element_length(block->getItemLength());
      data_msg->set_element_size(block->getItemSize());
      for ( auto it = block->getExtFlagsBegin(); it != block->getExtFlagsEnd(); ++it ) {
        DataMsg_KeyValue* ext_flag = data_msg->add_ext_flag();
        ext_flag->set_key(it->first);
        ext_flag->set_value(it->second);
      }
      VLOG(1) << "Finish writing " << i;
    }
    blockIdx ++;
  }
}

void Client::processOutput(TaskMsg &msg) {

  VLOG(1) << "Task finished, start reading output";

  if (num_outputs != msg.data_size()) {
    DLOG(ERROR) << "Exprected #output=" << num_outputs 
               << ", received #output=" << msg.data_size();
    throw commError("Num of output blocks mismatch");
  }

  for (int i=0; i<msg.data_size(); i++) {
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

    DataBlock_ptr block;
    if (output_blocks[i]) {
      block = output_blocks[i];

      // output already allocated, check if size matches
      if (num_items != block->getNumItems() ||
          item_length != block->getItemLength() ||
          item_size != block->getItemSize())
      {
        LOG_IF(ERROR, VLOG_IS_ON(1)) << "Size of output " << i << " does not match the allocated one";
        throw commError("Output size mismatch");
      }
    }
    else {
      DataBlock_ptr new_block(new DataBlock(num_items, item_length, item_size));
      output_blocks[i] = new_block;
      block = new_block;
    }

    std::string path = data_msg.file_path();
    VLOG(1) << "Reading output from " << path;
    try {
      block->readFromMem(path);

      // delete memory map file after read
      if (!deleteFile(path)) {
        LOG_IF(WARNING, VLOG_IS_ON(1)) << "Cannot delete file for output " << i;
      }
    }
    catch (std::runtime_error &e) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "Failed to read data from file " << path;
      throw std::runtime_error("Failed to process output");
    }
  }
  VLOG(1) << "Finish reading output";
}
} // namespace blaze
