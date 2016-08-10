#define LOG_HEADER "main"
#include <glog/logging.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <iomanip>

#include  "Client.h"

using namespace blaze;


typedef std::pair<std::string, float> Prediction;

static bool PairCompare(const std::pair<float, int>& lhs,
		const std::pair<float, int>& rhs) {
	return lhs.first > rhs.first;
}

 /* Return the indices of the top N values of vector v. */
static std::vector<int> Argmax(float *output, int feature_size, int N) {
	std::vector<std::pair<float, int> > pairs;
	for (size_t i = 0; i < feature_size; ++i)
		pairs.push_back(std::make_pair(output[i], i));
	std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);

	std::vector<int> result;
	for (int i = 0; i < N; ++i)
		result.push_back(pairs[i].second);

	return result;
}


class VGGClient: public Client {
	public:
		VGGClient(): Client("VGG-16", 1, 1) {;}

		void compute() {
			throw std::runtime_error("No CPU implementation");
		}

		cv::Mat MatResize(const cv::Mat &img);
		void MatFlattern(const cv::Mat &sample_resized, float *input);
		void setGeometry(const int &width, const int &height);
		void setNumChannels(const int &num_channels);
		void setLabels(const std::string &label_file);
		void printPredictions(float *output_ptr, int num_images, int feature_size);

	private:
		cv::Size input_geometry_;
		int num_channels_;
		std::vector<std::string> labels_;
};

void VGGClient::MatFlattern(const cv::Mat &sample_resized, float *input)
{
	int im_height = sample_resized.rows;
	int im_width = sample_resized.cols;

	for (int i = 0; i < im_height; i++) {
		for (int j = 0; j < im_width; j++) {
			for (int k = 0; k < num_channels_; k++) {
				//		std::cout << "(" << i << "," << j << "," << k << ")" << std::endl;
				//	input_data[i * im_width + j * num_channels] = channel.at<float>(i, j);

				input[i * im_width * num_channels_ + j * num_channels_ + k] = sample_resized.at<cv::Vec3b>(i, j)[k];
				//	img_ptr[i * im_width * 3 + j * 3 + 0] = img.at<float>(j, i, 0);
				//	img_ptr[i * im_width * 3 + j * 3 + 1] = img.at<float>(j, i, 1);
				//	img_ptr[i * im_width * 3 + j * 3 + 2] = img.at<float>(j, i, 2);
			}
		}
	}
}

void VGGClient::setGeometry(const int &width, const int &height)
{
	input_geometry_ = cv::Size(width, height);
}

void VGGClient::setNumChannels(const int &num_channels)
{
	num_channels_ = num_channels;
}

cv::Mat VGGClient::MatResize(const cv::Mat &img)
{
	//	std::cout << __FUNCTION__ << std::endl;
	/* Convert the input image to the input image format of the network. */
	cv::Mat sample;
	if (img.channels() == 3 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
	else if (img.channels() == 4 && num_channels_ == 1)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
	else if (img.channels() == 4 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
	else if (img.channels() == 1 && num_channels_ == 3)
		cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
	else
		sample = img;

	cv::Mat sample_resized;
	if (sample.size() != input_geometry_)
		cv::resize(sample, sample_resized, input_geometry_);
	else
		sample_resized = sample;

	return sample_resized;
}

void VGGClient::setLabels(const std::string &label_file)
{
	std::ifstream labels(label_file.c_str());                                                
	std::string line;
	while (std::getline(labels, line))                                                       
		labels_.push_back(std::string(line));    
}

void VGGClient::printPredictions(float *output_ptr, int num_images, int feature_size)
{

	for (int i = 0; i < num_images; i++) {
		float *output = output_ptr + i * feature_size;
		std::vector<int> maxN = Argmax(output, feature_size, 5);
		std::vector<Prediction> predictions;

		for (int j = 0; j < 5; j++) {
			int idx = maxN[j];
			predictions.push_back(std::make_pair(labels_[idx], output[idx]));
		}
		
		std::cout << "#img=" << i << std::endl;

		for (size_t j = 0; j < predictions.size(); ++j) {
			Prediction p = predictions[j];
			std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
				<< p.first << "\"" << std::endl;
		}
	}
}

int main(int argc, char** argv) {

	// GLOG configuration
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = 1;
	FLAGS_v = 2;

	if (argc < 2) {
		printf("USAGE: %s img_list.txt labels.txt\n", argv[0]);
		return -1;
	}



	/*
	   std::cout << CV_LOAD_IMAGE_UNCHANGED << std::endl;
	   std::cout << CV_LOAD_IMAGE_GRAYSCALE << std::endl;
	   std::cout << CV_LOAD_IMAGE_COLOR << std::endl;
	   */


	//  std::cout << "rows=" << img.rows << " " << "cols=" << img.cols << " " << "channels=" << img.channels() << std::endl;

	try {
		VGGClient client;

		std::ifstream fin;

		std::string img_list_file = argv[1];

		int im_height;
		int im_width;
		int im_num_channels;
		int im_size;
		int num_images;
		int feature_size;
		int batch_size;

		std::string file = argv[1];
		std::string img_name;

		im_height = 224;
		im_width = 224;
		im_num_channels = 3;
		im_size = im_height * im_width * im_num_channels;
		feature_size = 1000;
		batch_size = 10;

		client.setGeometry(im_width, im_height);
		client.setNumChannels(im_num_channels);

		num_images = 0;
		fin.open(img_list_file.c_str());
		while (fin >> img_name) {
			num_images++;
		}
		fin.close();

		std::cout << num_images << std::endl;

		float* input  = (float*)client.createInput(
				0, num_images, im_size, sizeof(float), BLAZE_INPUT);


		//	unsigned char *img_data = (unsigned char *)img.data;

		//	int r, g, b;

		num_images = 0;
		fin.open(img_list_file.c_str());
		while (fin >> img_name) {
			cv::Mat img = cv::imread(img_name, -1);
			//		std::vector<cv::Mat> input_channels;

			cv::Mat sample_resized;
			//	cv::Mat copy_img = img;

			sample_resized = client.MatResize(img);


			client.MatFlattern(sample_resized, input + num_images * im_size);

			/*
			   int num_channels = inpu_channels.size();
			//	float *input_data = input;

			for (int k = 0; k < num_channels; k++) {
			cv::Mat channel = input_channels[k];
			for (int i = 0; i < im_height; i++) {
			for (int j = 0; j < im_width; j++) {
			//		std::cout << "(" << i << "," << j << "," << k << ")" << std::endl;
			//	input_data[i * im_width + j * num_channels] = channel.at<float>(i, j);
			input[i * im_width + j] = channel.at<float>(i, j);
			//	img_ptr[i * im_width * 3 + j * 3 + 0] = img.at<float>(j, i, 0);
			//	img_ptr[i * im_width * 3 + j * 3 + 1] = img.at<float>(j, i, 1);
			//	img_ptr[i * im_width * 3 + j * 3 + 2] = img.at<float>(j, i, 2);
			}
			}
			//		input_data += im_height * im_width;
			input += im_height * im_width;
			}
			*/
			num_images++;
		}
		fin.close();

	//	std::string label_file = "/curr/xuechao/tools/caffe/data/ilsvrc12/synset_words.txt";
		std::string label_file = argv[2];
		client.setLabels(label_file);

		// start computation
		client.start();

		float* output_ptr = (float*)client.getOutputPtr(0);

		client.printPredictions(output_ptr, num_images, feature_size);

	}

	catch (std::exception &e) {
		printf("%s\n", e.what());
		return -1;
	}

	return 0;
}
