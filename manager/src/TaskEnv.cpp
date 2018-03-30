#include <boost/smart_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lockable_adapter.hpp>

#include "blaze/Block.h"
#include "blaze/TaskEnv.h"

namespace blaze {

DataBlock_ptr TaskEnv::createBlock(
    int num_items, 
    int item_length,
    int item_size, 
    int align_width, 
    int flag)
{
  DataBlock_ptr block(new DataBlock(
        num_items, item_length, item_size, align_width, flag));
  return block;
}

DataBlock_ptr TaskEnv::createBlock(
    char bankID,
    int num_items, 
    int item_length,
    int item_size, 
    int align_width, 
    int flag)
{
  DataBlock_ptr block(new DataBlock(bankID,
        num_items, item_length, item_size, align_width, flag));
  return block;
}

DataBlock_ptr TaskEnv::createBlock(const DataBlock& block) {
  DataBlock_ptr bp(new DataBlock(block));
  return bp; 
}

}
