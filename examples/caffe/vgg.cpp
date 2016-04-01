#include <caffe/caffe.hpp>
#include <cstdint>
#include <stdexcept>

#include "Task.h" 
#include "CaffeEnv.h"

using namespace blaze;

class VGG : public Task {
public:

  // extends the base class constructor
  // to indicate how many input blocks
  // are required
  VGG(): Task(1) {;}

  // overwrites the compute function
  virtual void compute() {

    // dynamically cast the TaskEnv to OpenCLEnv
    CaffeEnv* caffe_env = (CaffeEnv*)getEnv();

    // get input data dimensions
    int num_images = getInputNumItems(0);
    int image_size = getInputLength(0) / num_images;
    int feature_size = 1000;
    int batch_size = 10;

    if (image_size != 224*224*3) {
      throw std::runtime_error("Invalid image size");
    }

    // get the pointer to input/output data
    float* im   = (float*)getInput(0);
    float* feat = (float*)getOutput(0, 
        feature_size, num_images, sizeof(float));

    // perform computation
    caffe::Net<float>* net_ = caffe_env->getNet();
    const std::vector<boost::shared_ptr<caffe::Blob<float> > >& 
      blobs = net_->blobs();

    for (int i=0; i<num_images; i+=batch_size) {
      float* input  = im + image_size*i;
      float* output = feat + feature_size*i;

      int input_size = num_images-i >= batch_size ?
                        batch_size : num_images-i;
      int output_size = num_images-i >= batch_size ?
                        batch_size : num_images-i;

      switch(caffe::Caffe::mode()) {
        case caffe::Caffe::CPU:
          caffe::caffe_copy(input_size, input,
              blobs[0]->mutable_cpu_data());
          break;
        case caffe::Caffe::GPU:
          caffe::caffe_copy(input_size, input,
              blobs[0]->mutable_gpu_data());
          break;
      }

      net_->Forward();

      switch(caffe::Caffe::mode()) {
        case caffe::Caffe::CPU:
          caffe::caffe_copy(output_size, 
              blobs[blobs.size()-1]->cpu_data(),
              output);
          break;
        case caffe::Caffe::GPU:
          caffe::caffe_copy(output_size, 
              blobs[blobs.size()-1]->gpu_data(),
              output);
          break;
      }
    }
  }
};

// define the constructor and destructor for dlopen()
extern "C" Task* create() {
  return new VGG();
}

extern "C" void destroy(Task* p) {
  delete p;
}
