#define LOG_HEADER "main"
#include <glog/logging.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>

#include  "Client.h"

using namespace blaze;

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

private:
  cv::Size input_geometry_;
  int num_channels_;
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

	/*
	std::cout << img.rows << std::endl;
	std::cout << img.cols << std::endl;
	std::cout << img.dims << std::endl;
	std::cout << img.channels() << std::endl;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			for (int k = 0; k < img.channels(); k++) {
				std::cout << img.at<float>(i, j, k) << std::endl;
			//	std::cout << sample.at<float>(i, j, k) << std::endl;
			}
		}
	}
	*/

	cv::Mat sample_resized;
	if (sample.size() != input_geometry_)
		cv::resize(sample, sample_resized, input_geometry_);
	else
		sample_resized = sample;

	/*
	std::cout << sample_resized.rows << std::endl;
	std::cout << sample_resized.cols << std::endl;
	std::cout << sample_resized.channels() << std::endl;
	for (int i = 0; i < sample_resized.rows; i++) {
		for (int j = 0; j < sample_resized.cols; j++) {
			for (int k = 0; k < sample_resized.channels(); k++) {
				std::cout << sample_resized.at<float>(i, j, k);
			}
		}
	}
	*/

	return sample_resized;
}

int main(int argc, char** argv) {

  // GLOG configuration
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;
  FLAGS_v = 2;

  if (argc < 2) {
    printf("USAGE: %s img_list.jpg\n", argv[0]);
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

	std::string file = argv[1];
	std::string img_name;

	im_height = 224;
	im_width = 224;
	im_num_channels = 3;
	im_size = im_height * im_width * im_num_channels;

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

		/*
		std::cout << copy_img.rows << std::endl;
		std::cout << copy_img.cols << std::endl;
		std::cout << copy_img.channels() << std::endl;
		for (int i = 0; i < copy_img.rows; i++) {
			for (int j = 0; j < copy_img.cols; j++) {
				for (int k = 0; k < copy_img.channels(); k++) {
					std::cout << copy_img.at<float>(i, j, k) << std::endl;
					//	std::cout << sample.at<float>(i, j, k) << std::endl;
				}
			}
		}
		*/

		std::cout << "img.rows=" << img.rows << std::endl;
		std::cout << "img.cols=" << img.cols << std::endl;
		std::cout << "img.channels=" << img.channels() << std::endl;
		std::cout << "img.dims=" << img.dims << std::endl;
		std::cout << "img.type=" << img.type() << std::endl;
		std::cout << "img.step=" << img.step << std::endl;
		std::cout << "img.elemSize=" << img.elemSize() << std::endl;
		std::cout << "img.elemSize1=" << img.elemSize1() << std::endl;

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

	// start computation
	client.start();

	//    float* output_ptr = (float*)client.getOutputPtr(0);
  }
  catch (std::exception &e) {
	  printf("%s\n", e.what());
	  return -1;
  }

  return 0;
}
