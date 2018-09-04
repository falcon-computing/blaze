#ifndef CLIENT_H
#define CLIENT_H

#include <gtest/gtest_prod.h>
#include <string>
#include <utility>

#include "task.pb.h"
#include "BaseClient.h"
#include "Common.h"

#define BLAZE_INPUT         0
#define BLAZE_INPUT_CACHED  1
#define BLAZE_SHARED        2

using namespace boost::asio;

namespace blaze {

typedef boost::shared_ptr<boost::thread> client_event;

typedef struct {
  int num_items;
  int item_length;
  int data_width;
} BlockSizeInfo;

class Client : public BaseClient {
  FRIEND_TEST(ClientTests, CheckBlockAllocation);
  FRIEND_TEST(ClientTests, CheckPrepareRequest);
 public:
  Client(std::string _acc_id, 
         int _num_inputs, 
         int _num_outputs,
         int port = 1027);

  void* createInput( int idx,
                     int num_items, 
                     int item_length, 
                     int data_width, 
                     int type = BLAZE_INPUT);

  void* createOutput(int idx,
                     int num_items, 
                     int item_length, 
                     int data_width);

  // copy data to an input block from a pointer,
  // allocate the space if the block has not been created
  void setInput(int idx, void* src, 
                int num_items = 0,
                int item_length = 0, 
                int data_width = 0,
                int type = BLAZE_INPUT);

  // get the data pointer and information of an input block
  void* getInputPtr(int idx);
  int   getInputNumItems(int idx);
  int   getInputLength(int idx);

  // get the data pointer and information of an output block
  void* getOutputPtr(int idx);
  int   getOutputNumItems(int idx);
  int   getOutputLength(int idx);

  // start client and wait for results
  // NOTE: in current version the call is blocking no matter
  // what input is provided
  void start(bool blocking = true);

  // pure virtual method to be overloaded
  virtual void compute() = 0;

 protected:
  std::vector<DataBlock_ptr> input_blocks_;
  std::vector<DataBlock_ptr> output_blocks_;
  std::vector<std::pair<int64_t, bool> > block_info_;

  std::vector<BlockSizeInfo> input_size_info_;

 private:
  // helper functions in communication flow
  void prepareRequest(TaskMsg &msg);
  void prepareData(TaskMsg &data_msg, TaskMsg &reply_msg);
  void processOutput(TaskMsg &msg);

  std::string acc_id;
  std::string app_id;

  // input/output data blocks
  int num_inputs;
  int num_outputs;

  bool send_ack_;
};

} // namespace blaze

#endif
