#include <climits>

#ifdef NDEBUG
#define LOG_HEADER "BlockManager"
#endif
#include <glog/logging.h>

#include "blaze/Block.h"
#include "blaze/BlockManager.h"
#include "blaze/Platform.h"
#include "blaze/TaskEnv.h"

namespace blaze {

BlockManager::~BlockManager() {
  DVLOG(2) << "Destroying block manager";
}

DataBlock_ptr BlockManager::create_block(
      int num_items, int item_length, int item_size, 
      int align_width, DataBlock::Flag flag)
{
  // TODO: should record the size of the data for 
  // memory usage monitoring
  //
  // NOTE: does not check lock() since BlockManager
  // should be destroyed before Platform is destroyed
  return platform->getEnv().lock()->create_block(
                num_items, item_length, item_size,
                align_width, flag);
}

DataBlock_ptr BlockManager::create_block(
      std::string path,
      int num_items, int item_length, int item_size, 
      int align_width, DataBlock::Flag flag)
{
  return platform->getEnv().lock()->create_block(path,
                num_items, item_length, item_size,
                align_width, flag);
}

// create a block if it does not exist in the manager
// return true if a new block is created
bool BlockManager::get_alloc(
    int64_t tag, 
    DataBlock_ptr &block,
    int num_items,
    int item_length,
    int item_size,
    int align_width)
{
  // guarantee exclusive access
  boost::lock_guard<BlockManager> guard(*this);

  if (!contains(tag)) {

    // create block never throw exception
    DataBlock::Flag flag = DataBlock::SHARED;
    if (tag < 0) {
      flag = DataBlock::OWNED;
    }
    // create a block to share
    // only happens when creating a scratch block
    block = create_block(num_items, item_length, 
        item_size, align_width, flag);
    
    try {
      do_add(tag, block);

      return true;
    }
    catch (std::runtime_error &e) {
      DLOG(ERROR) << "Cannot add block " << tag <<
        ": " << e.what();
      return false;
    }
  }
  else {
    if (tag < 0) {
      block = scratchTable[tag];
    }
    else {
      block = cacheTable[tag].second;
    }
    return false;
  }
}

DataBlock_ptr BlockManager::get(int64_t tag) {

  // guarantee exclusive access
  boost::lock_guard<BlockManager> guard(*this);

  // log info
  VLOG(2) << "Requesting block " << tag;
  
  if (tag < 0) {
    if (scratchTable.find(tag) == scratchTable.end()) {
      return NULL_DATA_BLOCK;
    }
    else {
      return scratchTable[tag];
    }
  }
  else {
    if (cacheTable.find(tag) == cacheTable.end()) {
      return NULL_DATA_BLOCK;
    }
    else {
      // accumulate the access count for the block
      // NOTE: better performance if this is being
      // done asynchronously
      update(tag);

      return cacheTable[tag].second;
    }
  }
}

void BlockManager::do_add(int64_t tag, DataBlock_ptr block) {

  // check if block already exists
  if (contains(tag)) {
    return;
  }
  if (tag < 0) { // scratch block
    if (scratchSize + block->getSize() >= maxScratchSize) {

      // cannot add because running out of space
      throw std::runtime_error("Cannot add broadcast block");
    }
    VLOG(2) << "Adding block " << tag << " to scratch";

    // add the index to cacheTable
    scratchTable.insert(std::make_pair(tag, block));

    // increase the current scratchSize
    scratchSize += block->getSize();
  }
  else {
    // log info
    VLOG(2) << "Adding block " << tag << " to cache";

    // do not add if current block is too big
    if (block->getSize() > maxCacheSize) {
      throw std::runtime_error("no space left"); 
    }

    // remove block from cache if no space left
    while (cacheSize + block->getSize() > maxCacheSize) {
      evict();
    }

    // add the index to cacheTable
    time_t timeNow;
    time(&timeNow);
    cacheTable.insert(std::make_pair(tag, std::make_pair(timeNow, block)));

    // increase the current cacheSize
    cacheSize += block->getSize();
  }
}

void BlockManager::remove(int64_t tag) {

  // guarantee exclusive access
  boost::lock_guard<BlockManager> guard(*this);

  // can only remove scratch data
  if (tag < 0) {

    if (scratchTable.find(tag) == scratchTable.end()) {
      // no data match tag; 
      // do nothing
    }
    else {
      DataBlock_ptr block = scratchTable[tag];
      scratchSize -= block->getSize();

      //delete block;
      scratchTable.erase(tag);

      VLOG(2) << "Removed block " << tag;
    }
  }
}

void BlockManager::evict() {

  if (cacheTable.size()==0) {
    throw std::runtime_error("no space left"); 
  }
   
  // find the block that has the least access count
  time_t min_ts;
  time(&min_ts);

  std::map<int64_t, std::pair<time_t, DataBlock_ptr> >::iterator 
    min_idx = cacheTable.begin();

  std::map<int64_t, std::pair<time_t, DataBlock_ptr> >::iterator iter; 
  for (iter = cacheTable.begin(); 
       iter != cacheTable.end(); 
       iter ++)
  {
    if (difftime(min_ts, iter->second.first) > 0) {
      min_ts  = iter->second.first;
      min_idx = iter;
    }
  }

  int size = min_idx->second.second->getSize();
  int64_t tag = min_idx->first;

  // remove the block 
  cacheTable.erase(min_idx);

  // decrease the current cacheSize
  cacheSize -= size;

  // log info
  VLOG(1) << "Evicted block " << tag;
}

// update access count for a block, 
// and then update cacheTable for new indexes
void BlockManager::update(int64_t tag) {

  time_t ts;
  time(&ts);

  // log info
  VLOG(2) << "Update block " << tag;

  cacheTable[tag].first = ts;
}

}
