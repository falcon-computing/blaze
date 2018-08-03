#include "TestCommon.h"
#include "blaze/Client.h"

namespace blaze {

class TestClient : public Client {
public:
  TestClient(int ni, int no): 
      Client("test", ni, no, app_port) {;}

  void compute() {
    throw cpuCalled("");
  }
};

bool runArrayTest() {
  // prepare input
  TestClient client(2, 1); 

  int num_samples = 128;
  int feature_size = 1024;
  int data_size = num_samples*feature_size;

  double* data_ptr    = (double*)client.createInput(0, num_samples, feature_size, sizeof(double), BLAZE_INPUT);
  double* weight_ptr  = (double*)client.createInput(1, feature_size, 1, sizeof(double), BLAZE_INPUT);
  double* output_base = new double[num_samples*feature_size];
  
  // setup input with random data
  for (int i=0; i<num_samples; i++) {
    for (int j=0; j<feature_size; j++) {
      data_ptr[i*feature_size+j] = (double)rand()/RAND_MAX;
    }
  }
  for (int i=0; i<feature_size; i++) {
    weight_ptr[i] = (double)rand()/RAND_MAX;
  }
  
  // start computation
  try {
    client.start();
    return true;
  } catch (cpuCalled &e) {
    return false;
  }

  // compute baseline results
  for (int i = 0; i < num_samples; i++) {
    for (int j = 0; j < feature_size; j++) {
      output_base[i * feature_size + j] = 
        data_ptr[i * feature_size +j] + 
        weight_ptr[j];
    }
  }
  double* output_ptr = (double*)client.getOutputPtr(0);
  for (int k=0; k<data_size; k++) {
    if (abs(output_ptr[k] - output_base[k]) >= 1e-6) {
      return false;
    }
  }
  return true;
}

bool runLoopBack(int data_size) {
  // prepare input
  TestClient client(1, 1); 

  double* input_ptr = (double*)client.createInput(0, 1, data_size, sizeof(double), BLAZE_INPUT);
  
  // setup input with random data
  for (int k=0; k<data_size; k++) {
    input_ptr[k] = (double)rand()/RAND_MAX;
  }
  
  // start computation
  try {
    client.start();
    return true;
  } catch (cpuCalled &e) {
    return false;
  }

  // compare results
  double* output_ptr = (double*)client.getOutputPtr(0);
  for (int k=0; k<data_size; k++) {
    if (abs(output_ptr[k] - input_ptr[k]) >= 1e-6) {
      return false;
    }
  }
  return true;
}

bool runDelay(int data_size) {
  // prepare input
  TestClient client(1, 0); 

  double* input_ptr = (double*)client.createInput(0, 1, data_size, sizeof(double), BLAZE_INPUT);
  
  // setup input with random data
  for (int k=0; k<data_size; k++) {
    input_ptr[k] = (double)rand()/RAND_MAX;
  }
  
  // start computation
  try {
    client.start();
    return true;
  } catch (cpuCalled &e) {
    return false;
  }
}

bool runDelayWEst(uint64_t task_us, uint64_t cpu_us, uint64_t force_us) {
  // prepare input
  TestClient client(3, 0); 
  uint64_t* input_0 = (uint64_t*)client.createInput(0, 1, 1, sizeof(uint64_t), BLAZE_INPUT);
  uint64_t* input_1 = (uint64_t*)client.createInput(1, 1, 1, sizeof(uint64_t), BLAZE_INPUT);
  uint64_t* input_2 = (uint64_t*)client.createInput(2, 1, 1, sizeof(uint64_t), BLAZE_INPUT);

  input_0[0] = task_us;
  input_1[0] = cpu_us;
  input_2[0] = force_us;

  try {
    client.start();
    return true;
  } catch (cpuCalled &e) {
    return false;
  }
}
}
