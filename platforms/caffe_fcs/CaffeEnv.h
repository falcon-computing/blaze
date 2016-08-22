#ifndef CAFFEENV_H
#define CAFFEENV_H

#include <boost/shared_ptr.hpp>
#include <caffe/caffe.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "TaskEnv.h"
#include "falconMLlib.h"

namespace blaze 
{
class CaffePlatform;
class CaffeEnv : public TaskEnv 
{
  friend class CaffePlatform;
  public:
  /*
    caffe::Net<float>* getNet() {
      if (!net_.lock()) {
        throw internalError("Request network is deleted");
      }
      else {
        return net_.lock().get();
      }
    }
	*/

    caffe::Net<float>* getNet();

	void setMean(const std::string &mean_file);
	cv::Mat getMean();
//	void setLabels(const std::string &label_file);
//	std::vector<std::string> getLabels();
//	void setCNNModel(boost::shared_ptr<caffe::Net<float> > net);
//	void setCNNModel(boost::weak_ptr<caffe::Net<float> > net);
	void setCNNModel();
	CNN4FPGA getCNNModel();
	void setFPGAModel(const std::string &bitstream_file);
	OpenCLFPGAModel getFPGAModel();
	void FPGAinit();

  private:
    void changeNetwork(boost::weak_ptr<caffe::Net<float> > net) {
      net_ = net;
    }

    boost::weak_ptr<caffe::Net<float> > net_;
//	std::vector<std::string> labels_;
	cv::Mat mean_;
	CNN4FPGA cnn_model_;
	OpenCLFPGAModel fpga;
};
} // namespace blaze
#endif
