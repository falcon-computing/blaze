//#include <ifstream>

#include "CaffeEnv.h"

namespace blaze {

	caffe::Net<float>* CaffeEnv::getNet() {
		if (!net_.lock()) {
			throw internalError("Request network is deleted");
		}
		else {
			return net_.lock().get();
		}
	}

void CaffeEnv::setMean(const std::string &mean_file)
{
	caffe::BlobProto blob_proto;
	ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

	/* Convert from BlobProto to Blob<float> */
	caffe::Blob<float> mean_blob;
	mean_blob.FromProto(blob_proto);

//	std::cout << "mean_blob.channels()=" << mean_blob.channels() << std::endl;

	/*
	CHECK_EQ(mean_blob.channels(), num_channels_)
		<< "Number of channels of mean file doesn't match input layer.";
		*/
	int num_channels = mean_blob.channels();

	/* The format of the mean file is planar 32-bit float BGR or grayscale. */
	std::vector<cv::Mat> channels;
	float* data = mean_blob.mutable_cpu_data();
	for (int i = 0; i < num_channels; ++i) {
		/* Extract an individual channel. */
		cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
		channels.push_back(channel);
		data += mean_blob.height() * mean_blob.width();
	}

	/* Merge the separate channels into a single image. */
	cv::Mat mean;
	cv::merge(channels, mean);

	/* Compute the global mean pixel value and create a mean image
	 * filled with this value. */
	cv::Scalar channel_mean = cv::mean(mean);
	caffe::Blob<float> *input_layer = net_.lock().get()->input_blobs()[0];
	cv::Size input_geometry = cv::Size(input_layer->width(), input_layer->height());
	mean_ = cv::Mat(input_geometry, mean.type(), channel_mean);
}

cv::Mat CaffeEnv::getMean()
{
	return mean_;
}

/*
void CaffeEnv::setLabels(const std::string &label_file)
{
	std::ifstream labels(label_file.c_str());
	CHECK(labels) << "Unable to open labels file " << label_file;
	std::string line;
	while (std::getline(labels, line))
		labels_.push_back(std::string(line));
}

std::vector<std::string> CaffeEnv::getLabels()
{
	return labels_;
}
*/
void CaffeEnv::setCNNModel()
{
//	boost::shared_ptr<caffe::Net<float>> net(getNet());
//	cnn_model_.setCNNModel(net);
	cnn_model_.setCNNModel(getNet());
//	std::cout << "cnn_model_.infm_len()=" << cnn_model_.infm_len() << std::endl;
}

CNN4FPGA CaffeEnv::getCNNModel()
{
	return cnn_model_;
}

void CaffeEnv::setFPGAModel(const std::string &bitstream_file)
{
	fpga.setFPGAModel(bitstream_file.c_str(), cnn_model_);
//	std::cout << "cnn_model_.infm_len()=" << cnn_model_.infm_len() << std::endl;
}

OpenCLFPGAModel CaffeEnv::getFPGAModel()
{
	return fpga;
}

void CaffeEnv::FPGAinit()
{
	fpga.FPGAinit();
}

} // namespace blaze
