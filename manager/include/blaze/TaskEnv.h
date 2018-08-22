#ifndef TASKENV_H
#define TASKENV_H

#include <string>
#include <utility>

#include "ConfigTable.h"
#include "Common.h"
#include "Block.h"

namespace blaze {

class TaskEnv {
public: 
  virtual DataBlock_ptr create_block(
      int num_items, int item_length, int item_size, 
      int align_width = 0, 
      DataBlock::Flag flag = DataBlock::SHARED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  virtual DataBlock_ptr create_block(std::string path,
      int num_items, int item_length, int item_size, 
      int align_width = 0,
      DataBlock::Flag flag = DataBlock::OWNED,
      ConfigTable_ptr conf = NULL_ConfigTable_ptr);

  virtual DataBlock_ptr create_block(const DataBlock& block);
};
}
#endif
