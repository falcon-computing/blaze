/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <map>
#include <vector>
#include <cstdlib>
#include <fstream>

#ifdef USEHDFS
#include "hdfs.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include "proto/task.pb.h"

#include "Block.h"
#include "TaskEnv.h"

#ifdef USE_OPENCL
#include "OpenCLBlock.h"
#include "OpenCLEnv.h"
#endif

namespace blaze {

// forward declaration of 
class TaskManager;
class Comm;

/**
 * Task is the base clase of an accelerator task
 * will be extended by user 
 */
class Task {

friend class TaskManager;
friend class Comm;

public:
  Task(TaskEnv *_env, int _num_input): 
    env(_env),
    status(NOTREADY), 
    num_input(_num_input),
    num_ready(0)
  {;}

  // main function to be overwritten by accelerator implementations
  virtual void compute() {;}

  // wrapper around compute(), added indicator for task status
  void execute() {
    try {
      compute();
      status = FINISHED;
    } catch (std::runtime_error &e) {
      status = FAILED; 
      throw e;
    }
  }

protected:

  // read one line from file and write to an array
  // and return the size of bytes put to a buffer
  virtual char* readLine(
      std::string line, 
      size_t &num_elements,
      size_t &num_bytes) 
  {
    num_bytes = 0; 
    num_elements = 0;
    return NULL;
  }

  char* getOutput(
      int idx, 
      int item_length, 
      int num_items,
      int data_width) 
  {

    if (idx < output_blocks.size()) {
      // if output already exists, return the pointer 
      // to the existing block
      return output_blocks[idx]->getData();
    }
    else {
      int length = num_items*item_length;

      // if output does not exist, create one

      DataBlock_ptr block;
      
      switch (env->getType()) {
        case AccType::CPU: 
        {
          DataBlock_ptr bp(new DataBlock(length, length*data_width));
          block = bp;
          break;
        }   
#ifdef USE_OPENCL
        case AccType::OpenCL:
        {
          DataBlock_ptr bp(new OpenCLBlock(
              reinterpret_cast<OpenCLEnv*>(env), 
              length, length*data_width));
          block = bp;
          break;
        }
#endif
        default: {
          DataBlock_ptr bp(new DataBlock(length, length*data_width));
          block = bp;
        }
      }
      block->setNumItems(num_items);

      output_blocks.push_back(block);

      return block->getData();
    }
  }

  int getInputLength(int idx) { 
    if (idx < input_blocks.size()) {
      return input_blocks[idx]->getLength(); 
    }
    else {
      throw std::runtime_error("getInputLength out of bound idx");
    }
  }

  int getInputNumItems(int idx) { 
    if (idx < input_blocks.size()) {
      return input_blocks[idx]->getNumItems() ; 
    }
    else {
      throw std::runtime_error("getInputNumItems out of bound idx");
    }
  }

  char* getInput(int idx) {
    if (idx < input_blocks.size()) {
      return input_blocks[idx]->getData();      
    }
    else {
      throw std::runtime_error("getInput out of bound idx");
    }
  }

  // pointer to task environment
  // accessable by the extended tasks
  TaskEnv *env;

private:

  void addInputBlock(int64_t partition_id, DataBlock_ptr block);

  DataBlock_ptr getInputBlock(int64_t block_id);

  // push one output block to consumer
  // return true if there are more blocks to output
  bool getOutputBlock(DataBlock_ptr &block);
   
  DataBlock_ptr onDataReady(const DataMsg &blockInfo);

  bool isReady();

  enum {
    NOTREADY,
    READY,
    FINISHED,
    FAILED,
    COMMITTED
  } status;

  // number of total input blocks
  int num_input;

  // number of input blocks that has data initialized
  int num_ready;

  // input and output data block
  std::vector<DataBlock_ptr> input_blocks;
  std::vector<DataBlock_ptr> output_blocks;

  std::map<int64_t, DataBlock_ptr> input_table;
};

typedef boost::shared_ptr<Task> Task_ptr;
}
#endif
