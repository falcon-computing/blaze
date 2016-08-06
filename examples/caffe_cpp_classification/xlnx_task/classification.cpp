#include <caffe/caffe.hpp>
#include <cstdint>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "CaffeEnv.h"

#include "classification.hpp" 

using namespace blaze;

/*
uint64_t getMs()
{
	struct timespec tr;
	clock_gettime(CLOCK_REALTIME, &tr);

	return (uint64_t)tr.tv_sec*1e3 + tr.tv_nsec/1e6;
}
*/

#if USE_PTHD //two pthreads
static void* cnnThd(void* arg){
  Classifier *p = (Classifier *)arg;

  float* img = p->img_ptr(p->cnn_img());
  float* feat = p->feat_ptr(p->cnn_feat());
  vector<float> result = p->fpga.FPGAexec(img);
  for(int i=0; i<result.size(); i++){
      feat[i] = result[i];
  }
//  std::cout << "Cnn read: img " << p->cnn_img() << " Cnn write: feat " << p->cnn_feat() << std::endl;
}

static void* annThd(void* arg){
  Classifier *p = (Classifier *)arg;

  float* feat = p->feat_ptr(p->ann_feat());
  float* fpga_result = p->fpga_result_ptr(p->ann_result());
  p->cnn_model.ann(p->net_, feat, fpga_result);
//  std::cout << "Ann read: feat " << p->ann_feat() << " Ann write: result " << p->ann_result() << std::endl;
}
#endif

VGG::VGG()
	:Task(1)
{
}

void VGG::compute() {

	// get input data dimensions
	int num_images = getInputNumItems(0);
	int image_size = getInputLength(0) / num_images;
	int feature_size = 1000;
	int batch_size = 1;

	int im_height = 224;
	int im_width = 224;
	int im_num_channels = image_size / (im_height * im_width);


	if (image_size != 224 * 224 * 3) {
		throw std::runtime_error("Invalid image size");
	}

    // dynamically cast the TaskEnv to OpenCLEnv
    CaffeEnv* caffe_env = (CaffeEnv*)getEnv();
	caffe::Net<float> *net = caffe_env->getNet();
//	boost::shared_ptr<caffe::Net<float>> net(caffe_env->getNet());

	// Just work around
	std::string mean_file = "/curr/xuechao/prog/caffe_fcs/data/ilsvrc12/imagenet_mean.binaryproto";
	std::string label_file = "/curr/xuechao/prog/caffe_fcs/data/ilsvrc12/synset_words.txt";
	std::string bitstream = "/curr/xuechao/prog/caffe_fcs/fcs/examples/cpp_classification/sdaccel/myproj_resyn_16_xfcn/impl/vgg16.xclbin";
//	std::string bitstream = "/curr/xuechao/prog/caffe_falcon/examples/cpp_classification_driverTest/sdaccel/vgg16_unroll8.xclbin";

	Classifier Classifier(mean_file, label_file, bitstream, net);

	// get the pointer to input/output data
	float* im   = (float*)getInput(0);
	float* feat = (float*)getOutput(0, 
			feature_size, num_images, sizeof(float));


//	cv::Mat sample_resized(cv::Size(im_height, im_width), CV_8UC3);
	cv::Mat sample_resized(im_height, im_width, CV_8UC3);
	//		cv::Mat sample_resized(cv::Size(im_height, im_width), CV_8UC1);
	// perform computation
	for (int h = 0;  h < num_images; h += batch_size) {
		float* input  = im + image_size * h;
		float* output = feat + feature_size * h;

	//	for (int j = 0; j < /*image_size*/ 10; j++) {
	//		std::cout << input[j] << std::endl;
	//	}
	
		/*
		cv::Mat img;
		std::string file = "/curr/xuechao/prog/blaze/examples/caffe_cpp_classification/client/cat.jpg";
		img = cv::imread(file, -1);	
		CHECK(!img.empty()) << "Unable to decode image " << file;
		*/
	


		for (int i = 0; i < im_height; i++) {
			for (int j = 0; j < im_width; j++) {
				for (int k = 0; k < im_num_channels; k++) {
				//	std::cout << "(" << i << "," << j << "," << k << ")" << std::endl;
					sample_resized.at<cv::Vec3b>(i, j)[k] = input[i * im_width * im_num_channels + j * im_num_channels + k];
				}
			}
		}

		/*
		uint64_t start_ts, stop_ts; 
		double es;

		start_ts = getMs();
		*/

		std::vector<Prediction> predictions = Classifier.Classify(sample_resized);

		/*
		stop_ts = getMs();

		es = (double)(stop_ts - start_ts);

		printf("%.3lf ms\n", es);
		*/

		/* Print the top N predictions. */
		for (size_t i = 0; i < predictions.size(); ++i) {
			Prediction p = predictions[i];
			std::cout << std::fixed << std::setprecision(4) << p.second << " - \""
				<< p.first << "\"" << std::endl;
		}
	}
}

/*
Classifier::Classifier(const std::string& mean_file,
                       const std::string& label_file,
                       const std::string& bitstream,
					   boost::shared_ptr<caffe::Net<float>> net)
					   */
Classifier::Classifier(const std::string& mean_file,
                       const std::string& label_file,
                       const std::string& bitstream,
					   caffe::Net<float> *net)
{

	net_ = net;
//	std::cout << net_->num_inputs() << std::endl;
//	std::cout << net_->num_outputs() << std::endl;

#if USE_FPGA
  cnn_model.setCNNModel(net_);
  fpga.setFPGAModel(bitstream.c_str(), cnn_model);
  fpga.FPGAinit();
  printf("finish FPGA initilization\n");
#endif
#if USE_PTHD
  param_flag = 0;
  //img1 = new float[cnn_model.infm_len()];
  //img2 = new float[cnn_model.infm_len()];
  feat1 = new float[cnn_model.lastfm_len()];
  feat2 = new float[cnn_model.lastfm_len()];
  fpga_result1 = new float[1000];
  fpga_result2 = new float[1000];
#endif

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  caffe::Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
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
  CHECK_EQ(labels_.size(), output_layer->channels())
    << "Number of labels is different from the output layer dimension.";
}

static bool PairCompare(const std::pair<float, int>& lhs,
                        const std::pair<float, int>& rhs) {
  return lhs.first > rhs.first;
}

Classifier::~Classifier()
{
#if USE_PTHD
    //delete [] img1; 
    //delete [] img2;
    delete [] feat1;
    delete [] feat2;
    delete [] fpga_result1;
    delete [] fpga_result2;
#endif
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

#if USE_FPGA
  std::vector<float> output = FPGA_Predict(sample_resized);
#else
  std::vector<float> output = Predict(sample_resized);
#endif
//  std::vector<float> output = Predict(img);

  N = std::min<int>(labels_.size(), N);
  std::vector<int> maxN = Argmax(output, N);
  std::vector<Prediction> predictions;
  for (int i = 0; i < N; ++i) {
    int idx = maxN[i];
    predictions.push_back(std::make_pair(labels_[idx], output[idx]));
  }

  return predictions;
}

/* Load the mean file in binaryproto format. */
void Classifier::SetMean(const std::string& mean_file) {
	caffe::BlobProto blob_proto;
  ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

  /* Convert from BlobProto to Blob<float> */
  caffe::Blob<float> mean_blob;
  mean_blob.FromProto(blob_proto);
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

#if USE_FPGA
std::vector<float> Classifier::FPGA_Predict(const cv::Mat& sample_resized){
  Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_, input_geometry_.height, input_geometry_.width);

  // Forward dimension change to all layers.
  net_->Reshape();
  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);
  Preprocess(sample_resized, &input_channels);

  //FPGA Acceleration Test
  printf("FPGA Prediction Start\n");

#if USE_PTHD

    const float* fdata = net_->input_blobs()[0]->cpu_data();
    img1 = new float[cnn_model.infm_len()];
    for(int i=0; i<cnn_model.infm_len(); i++) {
      img1[i] = (float)fdata[i];
    }

    param_cnn_img = 1; 
    param_ann_result = 1;
    if(param_flag == 0){
        param_cnn_feat = 1; 
        param_ann_feat = 2;
        param_flag = 1;
    }
    else{
        param_cnn_feat = 2; 
        param_ann_feat = 1;   
        param_flag = 0;
    }
    int ret_thrd1 = pthread_create(&th1, NULL, cnnThd, (void*)this);
    int ret_thrd2 = pthread_create(&th2, NULL, annThd, (void*)this);
    if (ret_thrd1 != 0){ 
            printf("falied to create thd1\n"); }
    if (ret_thrd2 != 0) {
            printf("falied to create thd2\n"); }

    void* retval;
    int tmp1 = pthread_join(th1, &retval);
    if (tmp1 != 0) {
            printf("cannot join with Cnn thread\n"); }
    int tmp2 = pthread_join(th2, &retval);
    if (tmp2 != 0) {
            printf("cannot join with Ann thread\n"); }

    
    Blob<float>* output_layer = net_->output_blobs()[0];
    
    std::vector<float> output;
    for(int k =0; k<output_layer->channels(); k++) {
      output.push_back(fpga_result1[k]); 
    }
    delete [] img1;
    return output;
#else
  float fpga_result[1000];

  /*
  float *ddram = (float*)malloc(sizeof(float)*INFM);
  const float* fdata = net_->input_blobs()[0]->cpu_data();
  for(int i=0; i<INFM; i++) {
    ddram[i] = (float)fdata[i];
  }
  */
  float *ddram = (float*)malloc(sizeof(float)*cnn_model.infm_len());
  const float* fdata = net_->input_blobs()[0]->cpu_data();
  for(int i=0; i<cnn_model.infm_len(); i++) {
    ddram[i] = (float)fdata[i];
  }

  vector< float > result = fpga.FPGAexec(ddram);
  float* fpga_feat = (float*)malloc(sizeof(float)*result.size()); 
  for(int i=0; i<result.size(); i++){
    fpga_feat[i] = result[i]; 
  }
  cnn_model.ann(net_, fpga_feat, fpga_result);
  //ann(fpga_feat, fpga_result);

  Blob<float>* output_layer = net_->output_blobs()[0];

  std::vector<float> output;
  for(int k =0; k<output_layer->channels(); k++) {
    output.push_back(fpga_result[k]); 
  }

  free(ddram);
  //free(dddram);
  //free(feat_dev);
  free(fpga_feat);
  printf("FPGA Prediction finish\n");
  return output;
#endif
}
#endif


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

void Classifier::Preprocess(const cv::Mat& sample_resized,
                            std::vector<cv::Mat>* input_channels) {
  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;

//  std::cout << mean_.channels() << " " << mean_.size() << std::endl;
//  std::cout << sample_float.channels() << " " << sample_float.size() << std::endl;

  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}

// define the constructor and destructor for dlopen()
extern "C" Task* create() {
  return new VGG();
}

extern "C" void destroy(Task* p) {
  delete p;
}
