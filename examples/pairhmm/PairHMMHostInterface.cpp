#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>

#include "blaze/Timer.h"
#include "PairHMMHostInterface.h"

// Encode a scalar value to serialized data
template <typename T>
static inline void putT(void* buf, uint64_t & buf_idx, T value) {
  int len = sizeof(value);
  memcpy((char*)buf + buf_idx, reinterpret_cast<char*>(&value), len);
  buf_idx += len;
}

// Decode a scalar value from serialized data
template <typename T>
static inline void getT(const void* buf, uint64_t & buf_idx, T &value) {
  int len = sizeof(value);
  memcpy(reinterpret_cast<char*>(&value), (char*)buf + buf_idx, len);
  buf_idx += len;
}

// Store a string with its length to serialized data
static inline void putStr(void* buf, uint64_t & buf_idx, const char* str, size_t len) {
  memcpy((char*)buf + buf_idx, str, len);
  buf_idx += len;
}

// Retrieve a string from serialized data
static inline void getStr(const void* buf, uint64_t & buf_idx, char* &dst, size_t len) {
  if (len > 0) {
    dst = (char*)malloc(len+1);
    memcpy(dst, (char*)buf + buf_idx,  len);
    dst[len] = '\0'; // add trailing \0 for compatibility
    buf_idx += len;
  }
}

uint64_t serialize(void* buf, const read_t * reads, int num) {

  uint64_t buf_idx = 0;
  putT(buf, buf_idx, num);

  for (int i = 0; i < num; i++) {
    int len = reads[i].len; 
    putT(buf, buf_idx, len);

    putStr(buf, buf_idx, reads[i]._b, len);
    putStr(buf, buf_idx, reads[i]._q, len);
    putStr(buf, buf_idx, reads[i]._i, len);
    putStr(buf, buf_idx, reads[i]._d, len);
    putStr(buf, buf_idx, reads[i]._c, len);
  }
  return buf_idx;
}

uint64_t serialize(void* buf, const hap_t * haps, int num) {

  uint64_t buf_idx = 0;
  putT(buf, buf_idx, num);

  for (int i = 0; i < num; i++) {
    int len = haps[i].len;
    putT(buf, buf_idx, len);
    putStr(buf, buf_idx, haps[i]._b, len);
  }
  return buf_idx;
}

int deserialize(
    const void* buf, 
    read_t* &reads) 
{

  uint64_t buf_idx = 0;
  int num = 0;
  getT(buf, buf_idx, num);

  reads = (read_t*)malloc(num*sizeof(read_t));

  for (int i = 0; i < num; i++) {
    int len = 0;
    getT(buf, buf_idx, len);
    reads[i].len = len;

    getStr(buf, buf_idx, reads[i]._b, len);
    getStr(buf, buf_idx, reads[i]._q, len);
    getStr(buf, buf_idx, reads[i]._i, len);
    getStr(buf, buf_idx, reads[i]._d, len);
    getStr(buf, buf_idx, reads[i]._c, len);
  }
  return num;
}

int deserialize(
    const void* buf, 
    hap_t* &haps) 
{
  uint64_t buf_idx = 0;
  int num = 0;
  getT(buf, buf_idx, num);

  haps = (hap_t*)malloc(num*sizeof(hap_t));

  for (int i = 0; i < num; i++) {
    int len = 0;
    getT(buf, buf_idx, len);
    haps[i].len = len;

    getStr(buf, buf_idx, haps[i]._b, len);
  }
  
  return num;
}

