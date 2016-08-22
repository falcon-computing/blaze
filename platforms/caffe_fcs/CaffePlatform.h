#ifndef CAFFE_PLATFORM_H
#define CAFFE_PLATFORM_H

#include "Platform.h"

#include <caffe/caffe.hpp>
#include <map>
#include <string>
#include <stdexcept>


namespace blaze 
{
class CaffePlatform : public Platform 
{
  public:
    CaffePlatform(std::map<std::string, std::string> &conf_table);

    TaskEnv_ptr getEnv(std::string id);

    void addQueue(AccWorker &conf);
    void removeQueue(std::string id);

    void changeNetwork(std::string id);

  private:
    TaskEnv_ptr env_ptr_;
    std::string curr_network_;
    std::map<std::string, std::string> model_def_;
    std::map<std::string, std::string> model_wt_;
    std::map<std::string, std::string> model_mn_;
    std::map<std::string, std::string> model_bs_;

    boost::shared_ptr<caffe::Net<float> > net_;
};

extern "C" Platform* create(
    std::map<std::string, std::string> &config_table);

extern "C" void destroy(Platform* p);

} // namespace blaze
#endif
