#ifndef BLOCK_H
#define BLOCK_H

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/thread/lockable_traits.hpp> 
#include <map>
#include <stdio.h>
#include <string>
#include <utility>

#include "ConfigTable.h"
#include "Common.h"

/*
 * base class extendable to manage memory block
 * on other memory space (e.g. FPGA device memory)
 */

namespace blaze {

class DataBlock
: public boost::basic_lockable_adapter<boost::mutex>
{

public:
  typedef enum {
    OWNED, // means current owner is response for deletion
    SHARED // means other use will delete after use
  } Flag;

  DataBlock(std::string path,
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width = 0,
      Flag _flag = SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);
 
  // create basic data block to write to
  // path will be automatically defined
  DataBlock(int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width = 0,
      Flag _flag = SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);
   
  DataBlock(const DataBlock &block);

  virtual ~DataBlock();

  std::string get_path();

  // allocate data aligned to a given width
  //void alloc(int _align_width);

  // allocate data
  virtual void alloc() {;}

  // copy data from an array
  virtual void writeData(void* src, size_t _size);

  // copy data from an array with offset
  virtual void writeData(void* src, size_t _size, size_t offset);

  // write data to an array
  virtual void readData(void* dst, size_t size);

  // get the pointer to data
  virtual void* getData();

  // sample the items in the block by a mask
  // Deprecated
  virtual boost::shared_ptr<DataBlock> sample(char* mask);

  virtual void readFromMem(std::string path);
  virtual void writeToMem(std::string path);

  int getNumItems() { return num_items; }
  int getItemLength() { return item_length; }
  int getItemSize() { return item_size; }
  int getLength() { return length; }
  int getSize() { return size_; }
  Flag getFlag() { return flag_; }

  // status check of DataBlock needs to be exclusive
  bool isAllocated();
  bool isReady();
  void setReady();

protected:
  Flag flag_;       

  int item_length;  /* number of elements per data item */
  int item_size;    /* byte size per data item */
  int num_items;    /* number of data items per data block */
  int data_width;   /* byte size per element */
  int align_width;  /* align data width per data item */
  int length;       /* total number of elements */

  int64_t size_;     /* total byte size of the data block */

  bool is_aligned_;
  bool is_ready_;

  std::string                                           mm_file_path_;
  boost::shared_ptr<boost::interprocess::mapped_region> mm_region_;
  boost::shared_ptr<boost::interprocess::file_mapping>  mm_file_;

  ConfigTable_ptr conf_;

private:
  void calc_sizes(
      int _num_items, 
      int _item_length,
      int _item_size,
      int _align_width = 0);
  void map_region();
};

const DataBlock_ptr NULL_DATA_BLOCK;

}
#endif
