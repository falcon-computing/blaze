#define LOG_HEADER "main"
#include <glog/logging.h>

#include  "Client.h"

using namespace blaze;

class VGGClient: public Client {
public:
  VGGClient(): Client("VGG-16", 1, 1) {;}

  void compute() {
    throw std::runtime_error("No CPU implementation");
  }
};

int main(int argc, char** argv) {

  // GLOG configuration
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;
  FLAGS_v = 2;

  if (argc < 2) {
    printf("USAGE: %s <num_samples>\n", argv[0]);
    return -1;
  }

  int im_height  = 224;
  int im_width   = 224;
  int im_size    = im_height*im_width*3;
  int num_images = atoi(argv[1]);

  try {
    VGGClient client;

    float* image_ptr  = (float*)client.createInput(
        0, num_images, im_size, sizeof(float), BLAZE_INPUT);

    // start computation
    client.start();

    float* output_ptr = (float*)client.getOutputPtr(0);
  }
  catch (std::exception &e) {
    printf("%s\n", e.what());
    return -1;
  }

  return 0;
}
