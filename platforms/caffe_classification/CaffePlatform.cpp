#include "CaffeEnv.h"
#include "CaffePlatform.h"
#include "CaffeQueueManager.h"

namespace blaze 
{
CaffePlatform::CaffePlatform(
    std::map<std::string, std::string> &conf_table):
  Platform(conf_table),
  curr_network_("") 
{
  // get queue config
  int reconfig_timer = 500;  // default 500ms
  if (conf_table.find("reconfig timer") != conf_table.end())
  {
    reconfig_timer = stoi(conf_table["reconfig timer"]);
  }

  TaskEnv_ptr env_ptr(new CaffeEnv);
  env_ptr_ = env_ptr;

  QueueManager_ptr queue(
      new CaffeQueueManager(this, reconfig_timer)); 
  queue_manager = queue;
}

TaskEnv_ptr CaffePlatform::getEnv(std::string id) {
  return env_ptr_; 
}

void CaffePlatform::addQueue(AccWorker &conf) {
  std::string param_path;
  std::string model_path;
  std::string mean_path;
  std::string label_path;

  std::cout << conf.param_size() << std::endl;

  for (int i=0; i<conf.param_size(); i++) {
    if (conf.param(i).key().compare("param_path")==0) {
      param_path = conf.param(i).value();
    }
    if (conf.param(i).key().compare("model_path")==0) {
      model_path = conf.param(i).value();
    }
    if (conf.param(i).key().compare("mean_path")==0) {
      mean_path = conf.param(i).value();
    }
    if (conf.param(i).key().compare("label_path")==0) {
      label_path = conf.param(i).value();
    }
  }
  if (param_path.empty() || model_path.empty()) {
    throw invalidParam("Invalid configuration");
  }

  model_def_[conf.id()] = param_path;
  model_wt_[conf.id()] = model_path;
  model_mn_[conf.id()] = mean_path;
  model_lb_[conf.id()] = label_path;

  // add a TaskManager, and the scheduler should be started
  queue_manager->add(conf.id(), conf.path());

  // changeNetwork to switch to current accelerator
  try {
    changeNetwork(conf.id());
  }
  catch (internalError &e) {
    // if there is error, then remove acc from queue
    removeQueue(conf.id());
    throw e;
  }
}

void CaffePlatform::removeQueue(std::string id) {
  // asynchronously call queue_manager->remove(id)
  boost::thread executor(
      boost::bind(&QueueManager::remove, queue_manager.get(), id));

  model_def_.erase(id);
  model_wt_.erase(id);
  model_mn_.erase(id);
  model_lb_.erase(id);

  DLOG(INFO) << "Removed queue for " << id;
}

void CaffePlatform::changeNetwork(std::string id) {

  if (curr_network_.compare(id) != 0) {
    uint64_t start_t = getUs();

    if (!model_def_.count(id) || !model_wt_.count(id)) {
      throw internalError(std::string("No available model named ")+id);
    }
    boost::shared_ptr<caffe::Net<float> > net(
        new caffe::Net<float>(model_def_[id], caffe::TEST));

	// Load the network.
    net->CopyTrainedLayersFrom(model_wt_[id]);

    net_ = net;
    curr_network_ = id;

    CaffeEnv* env = dynamic_cast<CaffeEnv*>(env_ptr_.get());
    env->changeNetwork(net_);

	// Load the binaryproto mean file
	env->setMean(model_mn_[id]);
	// Load the labels
	env->setLabels(model_lb_[id]);


    DLOG(INFO) << "Switched network to " << id << ", which takes "
               << getUs() - start_t << "us";
  }
}

extern "C" Platform* create(
    std::map<std::string, std::string> &conf_table) 
{
  return new CaffePlatform(conf_table);
}

extern "C" void destroy(Platform* p) {
  delete p;
}
} // namespace blaze
