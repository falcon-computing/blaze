#include "Task.h" 

#include <CL/opencl.h>

#include "hls/cnn_cfg.hpp"

//if SIM
//#include "vgg16.hpp"

#include "falconMLlib.h"

#define USE_FPGA 1
#define USE_PTHD 0
#define FPGA_Verify 0
#define IMSHOW 0

using namespace blaze;

/* Pair (label, confidence) representing a prediction. */
typedef std::pair<std::string, float> Prediction;

class Classifier {
 public:
	 /*
  Classifier(const std::string& mean_file,
             const std::string& label_file,
             const std::string& bitstream,
			 boost::shared_ptr<caffe::Net<float>> net);
			 */
  Classifier(const std::string& mean_file,
             const std::string& label_file,
             const std::string& bitstream,
			 caffe::Net<float> *net);

  ~Classifier();

  std::vector<Prediction> Classify(const cv::Mat& sample_resized, int N = 5);

#if USE_FPGA 
 public:
  CNN4FPGA cnn_model;
  OpenCLFPGAModel fpga;
#endif

#if USE_PTHD
 public:
  pthread_t th1, th2;
  int param_cnn_img, param_cnn_feat, param_ann_feat, param_ann_result;
  bool param_flag;
  float* img1, *img2, *feat1, *feat2, *fpga_result1, *fpga_result2;

  int cnn_img(){
    return param_cnn_img;
  }
  int cnn_feat(){
    return param_cnn_feat;
  }
  int ann_feat(){
    return param_ann_feat;
  }
  int ann_result(){
    return param_ann_result;
  }
  float* img_ptr(int a){
    if(a==1) return img1; 
    else if(a==2) return img2;
    else return NULL;
  }
  float* feat_ptr(int a){
    if(a==1) return feat1; 
    else if(a==2) return feat2;
    else return NULL;
  }
  float* fpga_result_ptr(int a){
    if(a==1) return fpga_result1; 
    else if(a==2) return fpga_result2;
    else return NULL;
  }
#endif


 private:
  void SetMean(const std::string& mean_file);

  std::vector<float> Predict(const cv::Mat& sample_resized);

#if USE_FPGA
  std::vector<float> FPGA_Predict(const cv::Mat& sample_resized);
#endif

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);

  void Preprocess(const cv::Mat& sample_resized,
                  std::vector<cv::Mat>* input_channels);

 public:
//  boost::shared_ptr<caffe::Net<float> > net_;
  caffe::Net<float> *net_;
 private:
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
  std::vector<std::string> labels_;
};

class VGG : public Task {
public:

  // extends the base class constructor
  // to indicate how many input blocks
  // are required
  VGG();

  // overwrites the compute function
  virtual void compute();

private:
  boost::shared_ptr<caffe::Net<float> > net;

};
