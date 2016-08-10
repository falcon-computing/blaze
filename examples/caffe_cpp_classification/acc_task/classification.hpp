#include "Task.h" 

using namespace blaze;

typedef std::pair<std::string, float> Prediction;

class Classifier {
 public:
	 /*
  Classifier(const std::string& mean_file,
             const std::string& label_file,
			 boost::shared_ptr<caffe::Net<float>> net,
			 const int batch_size = 1);
			 */
	 /*
  Classifier(const std::string& mean_file,
             const std::string& label_file,
			 caffe::Net<float> *net,
			 const int batch_size = 1);
			 */
	 /*
  Classifier(const cv::Mat &mean,
             const std::vector<std::string> &labels,
			 boost::shared_ptr<caffe::Net<float>> net,
			 const int batch_size = 1);
			 */
  Classifier(const std::string& mean_file,
			 caffe::Net<float> *net,
			 const int batch_size = 1);

//  std::vector<Prediction> Classify(const cv::Mat& sample_resized, int N = 5);
//  std::vector< std::vector<Prediction> > Classify(const std::vector<cv::Mat> &sample_resized_batch, int N = 5);

  std::vector<float> Predict(const cv::Mat& sample_resized);
  std::vector<float> Predict(const std::vector<cv::Mat> &sample_resized_batch);

  void set_batch_size(const int batch_size);

 private:
  void SetMean(const std::string& mean_file);

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);
  void WrapInputLayer(std::vector< std::vector<cv::Mat> > *input_channels_batch);

  void Preprocess(const cv::Mat& sample_resized,
                  std::vector<cv::Mat>* input_channels);
  void Preprocess(const std::vector<cv::Mat> &sample_resized_batch,
                            std::vector< std::vector<cv::Mat> > *input_channels_batch);

 private:
 // boost::shared_ptr<caffe::Net<float> > net_;
  caffe::Net<float> *net_;
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
//  std::vector<std::string> labels_;
  int batch_size_;
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
