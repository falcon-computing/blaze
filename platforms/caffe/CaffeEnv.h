#ifndef CAFFEENV_H
#define CAFFEENV_H

#include <boost/shared_ptr.hpp>
#include <caffe/caffe.hpp>

#include "TaskEnv.h"

namespace blaze 
{
class CaffePlatform;
class CaffeEnv : public TaskEnv 
{
  friend class CaffePlatform;
  public:
    caffe::Net<float>* getNet() {
      if (!net_.lock()) {
        throw internalError("Request network is deleted");
      }
      else {
        return net_.lock().get();
      }
    }

  private:
    void changeNetwork(boost::weak_ptr<caffe::Net<float> > net) {
      net_ = net;
    }

    boost::weak_ptr<caffe::Net<float> > net_;
};
} // namespace blaze
#endif
