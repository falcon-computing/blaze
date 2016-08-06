#include <caffe/caffe.hpp>
#include <cstdint>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "classification.hpp" 
#include "CaffeEnv.h"

using namespace blaze;


VGG::VGG()
	:Task(1)
{
}

void VGG::compute() {

	// get input data dimensions
	int num_images = getInputNumItems(0);
	int image_size = getInputLength(0) / num_images;
	int feature_size = 1000;
	int batch_size = 10;

	int num_batches = num_images / batch_size;
	int num_marginal_images = num_images - num_batches * batch_size;

	int im_height, im_width;
	int im_num_channels;

	/*
	if (image_size != 224*224*3) {
		throw std::runtime_error("Invalid image size");
	}
	*/

    // dynamically cast the TaskEnv to OpenCLEnv
    CaffeEnv* caffe_env = (CaffeEnv*)getEnv();
	caffe::Net<float> *net = caffe_env->getNet();
//	boost::shared_ptr<caffe::Net<float>> net(caffe_env->getNet());


	/*
	std::cout << net << std::endl;
	std::cout << net->input_blobs()[0]->channels() << std::endl;
	std::cout << net->input_blobs()[0]->width() << std::endl;
	std::cout << net->input_blobs()[0]->height() << std::endl;
	*/

	// Just work around
	std::string mean_file = "/curr/xuechao/tools/caffe/data/ilsvrc12/imagenet_mean.binaryproto";
	std::string label_file = "/curr/xuechao/tools/caffe/data/ilsvrc12/synset_words.txt";

//	cv::Mat mean = caffe_env->getMean();
//	std::vector<std::string> labels = caffe_env->getLabels();
//	Classifier Classifier(mean_file, label_file, net);
	Classifier Classifier(mean_file, label_file, net, batch_size);
//	Classifier Classifier(mean, labels, net, batch_size);

	// get the pointer to input/output data
	float* im   = (float*)getInput(0);
	/*
	float* feat = (float*)getOutput(0, 
			feature_size, num_images, sizeof(float));
			*/

	std::cout << "num_images=" << num_images << std::endl;
	std::cout << "image_size=" << image_size << std::endl;

	im_height = 224;
	im_width = 224;
	im_num_channels = image_size / (im_height * im_width);

	std::vector<cv::Mat> sample_resized_batch;
//	cv::Mat sample_resized;
	cv::Mat sample_resized(im_height, im_width, CV_8UC3);
	// perform computation
//	int img_idx = 0;
	for (int i = 0; i < num_batches; i++) {
//		float* input  = im + image_size * i;
//		float* output = feat + feature_size*i;
		for (int j = 0; j < batch_size; j++) {

			std::cout << "#img=" << (i * batch_size + j) << std::endl;
			float* input  = im + (i * batch_size + j) * image_size;

			//	std::string file = "/curr/xuechao/prog/blaze/examples/caffe_cpp_classification/client/cat.jpg";
			// TODO: convert im into img
			//	CvtStringcvMat(im, &img);
			//	img = cv::imread(file, -1);	
			//		CHECK(!img.empty()) << "Unable to decode image " << file;

			for (int i = 0; i < im_height; i++) {
				for (int j = 0; j < im_width; j++) {
					for (int k = 0; k < im_num_channels; k++) {
						//			std::cout << "(" << i << "," << j << "," << k << ")" << std::endl;
						sample_resized.at<cv::Vec3b>(i, j)[k] = input[i * im_width * im_num_channels + j * im_num_channels + k];
					}
				}
			}

			sample_resized_batch.push_back(sample_resized);
		}
		std::vector< std::vector<Prediction> > predictions_batch = Classifier.Classify(sample_resized_batch);

		for (std::vector< std::vector<Prediction> >::iterator it = predictions_batch.begin(); it != predictions_batch.end(); it++) {
			std::vector<Prediction> predictions = *it;
			for (size_t i = 0; i < predictions.size(); ++i) {
				Prediction p = predictions[i];
				std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
					<< p.first << "\"" << std::endl;
			}
			std::cout << std::endl;
		}
	}
//	std::cout << "img_idx=" << img_idx << std::endl;
	// Process the marginal images
	Classifier.set_batch_size(1);
//	float *cur_im = im + num_batches * batch_size * image_size;
	for (int h = num_batches * batch_size; h < num_images; h++) {
		std::cout << "#img=" << h << std::endl;
		float * input = im + h * image_size;

//		cv::Mat sample_resized(im_height, im_width, CV_8UC3);

		for (int i = 0; i < im_height; i++) {
			for (int j = 0; j < im_width; j++) {
				for (int k = 0; k < im_num_channels; k++) {
					sample_resized.at<cv::Vec3b>(i, j)[k] = input[i * im_width * im_num_channels + j * im_num_channels + k];
				}
			}
		}

		std::vector<Prediction> predictions = Classifier.Classify(sample_resized);

		for (size_t i = 0; i < predictions.size(); ++i) {
			Prediction p = predictions[i];
			std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
				<< p.first << "\"" << std::endl;
		}
	}

	/*
	std::cout << net << std::endl;
	std::cout << net->input_blobs()[0]->channels() << std::endl;
	std::cout << net->input_blobs()[0]->width() << std::endl;
	std::cout << net->input_blobs()[0]->height() << std::endl;
	*/
}

/*
Classifier::Classifier(const cv::Mat &mean,
		const std::vector<std::string> &labels,
		boost::shared_ptr<caffe::Net<float>> net,
		const int batch_size)
		*/
/*
Classifier::Classifier(const std::string& mean_file,
		const std::string& label_file,
		boost::shared_ptr<caffe::Net<float>> net,
		const int batch_size)
		*/
Classifier::Classifier(const std::string& mean_file,
		const std::string& label_file,
		caffe::Net<float> *net,
		const int batch_size)
{
	/*
#ifdef CPU_ONLY
  Caffe::set_mode(Caffe::CPU);
#else
  Caffe::set_mode(Caffe::GPU);
#endif
*/

  /* Load the network. */
//  net_.reset(new Net<float>(model_file, TEST));
//  net_->CopyTrainedLayersFrom(trained_file);

	batch_size_ = batch_size;
	net_ = net;
//	std::cout << net_->num_inputs() << std::endl;
//	std::cout << net_->num_outputs() << std::endl;

    // dynamically cast the TaskEnv to OpenCLEnv
//    CaffeEnv* caffe_env = (CaffeEnv*)getEnv();
//	caffe::Net<float> *net = caffe_env->getNet();
//	boost::shared_ptr<caffe::Net<float>> net(caffe_env->getNet());

	/*
	mean_ = mean;
	labels_ = labels;
	net_ = net;
	*/

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  caffe::Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();

  std::cout << "num_channels=" << num_channels_ << std::endl;

  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  SetMean(mean_file);


  /* Load labels. */
  std::ifstream labels(label_file.c_str());
  CHECK(labels) << "Unable to open labels file " << label_file;
  std::string line;
  while (std::getline(labels, line))
    labels_.push_back(std::string(line));


  caffe::Blob<float>* output_layer = net_->output_blobs()[0];

  std::cout << "output_layer->channels()=" <<  output_layer->channels() << std::endl;

  CHECK_EQ(labels_.size(), output_layer->channels())
    << "Number of labels is different from the output layer dimension.";
}

static bool PairCompare(const std::pair<float, int>& lhs,
                        const std::pair<float, int>& rhs) {
  return lhs.first > rhs.first;
}

/* Return the indices of the top N values of vector v. */
static std::vector<int> Argmax(const std::vector<float>& v, int N) {
  std::vector<std::pair<float, int> > pairs;
  for (size_t i = 0; i < v.size(); ++i)
    pairs.push_back(std::make_pair(v[i], i));
  std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), PairCompare);

  std::vector<int> result;
  for (int i = 0; i < N; ++i)
    result.push_back(pairs[i].second);
  return result;
}

/* Return the top N predictions. */
std::vector<Prediction> Classifier::Classify(const cv::Mat& sample_resized, int N) {
	std::vector<float> output = Predict(sample_resized);

	N = std::min<int>(labels_.size(), N);
	std::vector<int> maxN = Argmax(output, N);
	std::vector<Prediction> predictions;
	for (int i = 0; i < N; ++i) {
		int idx = maxN[i];
		predictions.push_back(std::make_pair(labels_[idx], output[idx]));
	}

	return predictions;
}

std::vector< std::vector<Prediction> > Classifier::Classify(const std::vector<cv::Mat> &sample_resized_batch, int N) {

	std::vector<float> output_batch = Predict(sample_resized_batch);
	std::vector< std::vector<Prediction> > predictions_batch;
	int num_classes = labels_.size();

	N = std::min<int>(num_classes, N);
	for(int i = 0; i < sample_resized_batch.size(); i++) {
		std::vector<float> output(output_batch.begin() + i * num_classes, output_batch.begin() + (i + 1) * num_classes);
		std::vector<int> maxN = Argmax(output, num_classes);
		std::vector<Prediction> predictions;
		for (int j = 0; j < N; j++) {
			int idx = maxN[j];
			predictions.push_back(std::make_pair(labels_[idx], output[idx]));
		}
		predictions_batch.push_back(std::vector<Prediction>(predictions));
	}

	return predictions_batch;
}

/* Load the mean file in binaryproto format. */
void Classifier::SetMean(const std::string& mean_file) {
	caffe::BlobProto blob_proto;
  ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

  /* Convert from BlobProto to Blob<float> */
  caffe::Blob<float> mean_blob;
  mean_blob.FromProto(blob_proto);

  std::cout << "mean_blob.channels()=" << mean_blob.channels() << std::endl;

  CHECK_EQ(mean_blob.channels(), num_channels_)
    << "Number of channels of mean file doesn't match input layer.";

  /* The format of the mean file is planar 32-bit float BGR or grayscale. */
  std::vector<cv::Mat> channels;
  float* data = mean_blob.mutable_cpu_data();
  for (int i = 0; i < num_channels_; ++i) {
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
  mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
}

std::vector<float> Classifier::Predict(const cv::Mat& sample_resized) {
	caffe::Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_,
                       input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(sample_resized, &input_channels);

  net_->Forward();

  /* Copy the output layer to a std::vector */
  caffe::Blob<float>* output_layer = net_->output_blobs()[0];
  const float* begin = output_layer->cpu_data();
  const float* end = begin + output_layer->channels();

  return std::vector<float>(begin, end);
}

std::vector<float> Classifier::Predict(const std::vector<cv::Mat> &sample_resized_batch)
{
	caffe::Blob<float>* input_layer = net_->input_blobs()[0];
	input_layer->Reshape(batch_size_, num_channels_,
			input_geometry_.height, input_geometry_.width);

	/* Forward dimension change to all layers. */
	net_->Reshape();

	std::vector< std::vector<cv::Mat> > input_channels_batch;
	WrapInputLayer(&input_channels_batch);

	Preprocess(sample_resized_batch, &input_channels_batch);

	net_->Forward();

	/* Copy the output layer to a std::vector */
	caffe::Blob<float>* output_layer = net_->output_blobs()[0];
	const float* begin = output_layer->cpu_data();
	const float* end = begin + output_layer->channels() * sample_resized_batch.size();

	//  std::cout << output_layer->channels() << std::endl;

	return std::vector<float>(begin, end);
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Classifier::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
	caffe::Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void Classifier::WrapInputLayer(std::vector< std::vector<cv::Mat> > *input_channels_batch)
{
	caffe::Blob<float>* input_layer = net_->input_blobs()[0];

	int width = input_layer->width();
	int height = input_layer->height();
	int num = input_layer->num();
	float *input_data = input_layer->mutable_cpu_data();

	for (int i = 0; i < num; i++) {
		std::vector<cv::Mat> input_channels;
		for (int j = 0; j < input_layer->channels(); j++) {
			cv::Mat channel(height, width, CV_32FC1, input_data);
			input_channels.push_back(channel);
			input_data += width * height;
		}
		input_channels_batch->push_back(input_channels);
	}
}

void Classifier::Preprocess(const cv::Mat& sample_resized,
                            std::vector<cv::Mat>* input_channels) {
  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}

void Classifier::Preprocess(const std::vector<cv::Mat> &sample_resized_batch,
                            std::vector< std::vector<cv::Mat> > *input_channels_batch)
{
	for (int i = 0; i < sample_resized_batch.size(); i++) {
		cv::Mat sample_resized = sample_resized_batch[i];
		std::vector<cv::Mat> *input_channels = &(input_channels_batch->at(i));

		cv::Mat sample_float;
		if (num_channels_ == 3)
			sample_resized.convertTo(sample_float, CV_32FC3);
		else
			sample_resized.convertTo(sample_float, CV_32FC1);

		cv::Mat sample_normalized;
		cv::subtract(sample_float, mean_, sample_normalized);

		/* This operation will write the separate BGR planes directly to the
		 * input layer of the network because it is wrapped by the cv::Mat
		 * objects in input_channels. */
		cv::split(sample_normalized, *input_channels);

		/*
		   CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
		   == net_->input_blobs()[0]->cpu_data())
		   << "Input channels are not wrapping the input layer of the network.";
		   */
	}
}

void Classifier::set_batch_size(const int batch_size)
{
	batch_size_ = batch_size;
}

// define the constructor and destructor for dlopen()
extern "C" Task* create() {
  return new VGG();
}

extern "C" void destroy(Task* p) {
  delete p;
}
