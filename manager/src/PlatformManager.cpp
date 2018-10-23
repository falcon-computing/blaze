#include <boost/regex.hpp>
#include <dlfcn.h>
#include <fstream>
#include <stdexcept>

#ifdef NDEBUG
#define LOG_HEADER "PlatformManager"
#endif
#include <glog/logging.h>

#include "blaze/BlockManager.h"
#include "blaze/Platform.h"
#include "blaze/PlatformManager.h"
#include "blaze/QueueManager.h"
#include "blaze/TaskManager.h"

namespace blaze {

PlatformManager::PlatformManager(ManagerConf *conf): 
    platform_table(),
    acc_table(),
    platform_conf_table(),
    acc_conf_table()
{
  for (int i=0; i<conf->platform_size(); i++) {

    AccPlatform platform_conf = conf->platform(i);

    try {
      registerPlatform(platform_conf);
    }
    catch (std::runtime_error &e) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "Cannot create platform " << 
        platform_conf.id() << ": " << e.what();
    }
  }
}

PlatformManager::~PlatformManager() {
  // remove all platforms
  while (!platform_table.empty()) {
    auto it = platform_table.begin();
    DVLOG(1) << "Remove " << it->first << 
                  " before destroying platform manager";
    removePlatform(it->first);
  }
#ifndef NO_PROFILE
  if (gf_profile) {
    // print global timers when platform manager is destroyed
    ksight::ksight.print_total();
  }
#endif
}

void PlatformManager::registerPlatform(AccPlatform conf) {

  std::string id   = conf.id();

  // check platform id for special characters
  boost::regex special_char_test("\\w+", boost::regex::perl);
  if (!boost::regex_match(id.begin(), id.end(), special_char_test)) {
    LOG_IF(ERROR, VLOG_IS_ON(1)) << "Platform id [" << id << "] cannot contain " 
      << "special characters beside alphanumeric and '_'";
    throw std::runtime_error("Cannot register a new platform");
  }

  if (platform_conf_table.count(id)) {
    throw std::runtime_error("Duplicated platform found: " + id);
  }

  // write platform conf to platform_conf_table
  platform_conf_table[id] = conf; 

  // open the new platform
  openPlatform(id);

  // add accelerators to the platform
  for (int j = 0; j < conf.acc_size(); j++) {
    AccWorker acc_conf = conf.acc(j);
    try {
      registerAcc(id, acc_conf);          
    } 
    catch (std::exception &e) {
      LOG_IF(ERROR, VLOG_IS_ON(1)) << "Cannot create ACC " << acc_conf.id() <<
        ": " << e.what();
    }
  }
  DVLOG(1) << "Platform " << id << " is registered";
}

void PlatformManager::openPlatform(std::string id) {
  if (!platform_conf_table.count(id)) {
    throw std::runtime_error("Platform "+ id +" has not been registered yet.");
  }
  else if (platform_table.count(id)) {
    // skip platform creation if it already exists
    return;
  }

  AccPlatform conf = platform_conf_table[id];

  // generic Platform configuration
  std::string path  = conf.path();
  int cache_limit   = conf.cache_limit();
  int scratch_limit = conf.scratch_limit();

  std::string cache_loc = conf.has_cache_loc() ? 
    conf.cache_loc() : id;

  // extended Platform configurations
  std::map<std::string, std::string> conf_table;
  for (int i = 0; i < conf.param_size(); i++) {
    std::string conf_key   = conf.param(i).key();
    std::string conf_value = conf.param(i).value();
    conf_table[conf_key]   = conf_value;
  }

  // create Platform
  Platform_ptr platform = this->create(path, conf_table);

  platform_table.insert(std::make_pair(id, platform));

  // create block manager if this platform is 
  // using its own cache
  if (cache_loc.compare(id) != 0 && platform_table.count(cache_loc)) {
    platform->block_manager = platform_table[cache_loc]->block_manager;
    DVLOG(1) << "Use block manager on " << cache_loc;
  }
  else {
    if (cache_loc.compare(id) != 0) {
      LOG_IF(WARNING, VLOG_IS_ON(1)) << "Unspecified cache location, use private instead";
    }
    // if the cache is not shared with another platform
    // create a block manager in current platform
    platform->createBlockManager(
        (size_t)cache_limit << 20, 
        (size_t)scratch_limit << 20);

    DVLOG(1) << "Create a block manager for platform " << id;
  }

  // print extend configs
  if (!conf_table.empty()) {
    RVLOG(INFO, 1) << "Extra Configurations for the platform:";
    std::map<std::string, std::string>::iterator iter;
    for (iter  = conf_table.begin();
        iter != conf_table.end();
        iter ++)
    {
      RVLOG(INFO, 1) << "[""" << iter->first << """] = "
        << iter->second;
    }
  }

  // if acc_conf_table is not empty, it means we are restoring
  // a platform. therefore we need to register all previosu AccWorkers
  if (acc_conf_table.count(id)) {
    for (auto worker : acc_conf_table[id]) {
      DVLOG(1) << "Restoring old ACC: " << worker.id();
      registerAcc(id, worker);
    }
    acc_conf_table.erase(id);
    DVLOG(1) << "Removing " << id << " from acc_conf_table";
  }
}

void PlatformManager::removePlatform(std::string id) {
  if (!platform_table.count(id)) {
    DLOG(ERROR) << "Platform " << id << " is not active right now.";
    return;
  }
  else if (!platform_conf_table.count(id)) {
    DLOG(ERROR) << "Configuration missing for platform " << id;
    throw std::runtime_error("Unexpected platform configuration error");
  }

  Platform_ptr platform = platform_table[id];

  // remove all accelerators and add them to acc_conf_table
  //if (acc_conf_table.count(id)) {
  //  DLOG(ERROR) << "acc_conf_table " << id << " should be cleared after openPlatform";
  //  throw std::runtime_error("unexpected error in platform removal");
  //}

  std::vector<AccWorker> workers;
  for (auto it : platform->acc_table) {
    // remove queue based on AccWorker.id()
    std::string acc_id = it.second.id();
    removeAcc("", acc_id, id);
    DVLOG(1) << "Removing " << id << " from platform";

    // add AccWorker to acc_conf_table
    workers.push_back(it.second);
  }
  // add list of AccWorkers to acc_conf_table
  acc_conf_table[id] = workers;


  // erase platform from platform table
  // then when platform_ptr goes out-of-scope it should destruct
  platform_table.erase(id);

  DVLOG(1) << "Platform " << id << " is removed";
}

bool PlatformManager::accExists(std::string acc_id) {
  boost::lock_guard<PlatformManager> guard(*this);
  return acc_table.find(acc_id) != acc_table.end();
}

bool PlatformManager::platformExists(std::string platform_id) {
  // boost::lock_guard<PlatformManager> guard(*this);
  return platform_table.find(platform_id) != platform_table.end();
}

std::string PlatformManager::getPlatformIdByAccId(std::string acc_id) {
  if (acc_table.count(acc_id)) {
    return acc_table[acc_id];
  }
  else {
    return std::string();
  }
}

Platform* PlatformManager::getPlatformByAccId(std::string acc_id) {

  boost::lock_guard<PlatformManager> guard(*this);
  if (acc_table.find(acc_id) == acc_table.end()) {
    return NULL;
  } else {
    return platform_table[acc_table[acc_id]].get();
  }
}

Platform* PlatformManager::getPlatformById(std::string platform_id) {

  boost::lock_guard<PlatformManager> guard(*this);
  if (platform_table.find(platform_id) == platform_table.end()) {
    return NULL;
  } else {
    return platform_table[platform_id].get();
  }
}

TaskManager_ref PlatformManager::getTaskManager(std::string acc_id) {
  // lock all tables to guarantee exclusive access
  boost::lock_guard<PlatformManager> guard(*this);
  if (acc_table.find(acc_id) == acc_table.end()) {
    TaskManager_ref ret;
    return ret;
  } else {
    return platform_table[acc_table[acc_id]]->getTaskManager(acc_id);  
  }
}

void PlatformManager::registerAcc(
    std::string platform_id, 
    AccWorker &acc_conf) 
{
  // lock all tables to guarantee exclusive access
  boost::lock_guard<PlatformManager> guard(*this);

  DVLOG(1) << "Adding acc: " << acc_conf.id();

  // check if acc of the same already exists
  if (acc_table.find(acc_conf.id()) != acc_table.end()) {
    throw commError(
        "Accelerator already exists");
  }
  Platform_ptr platform = platform_table[platform_id];

  if (!platform) {
    throw commError(
        "Required platform does not exist");
  }

  // setup the task environment with ACC conf
  platform->addQueue(acc_conf);

  // add acc mapping to table
  acc_table.insert(std::make_pair(
        acc_conf.id(), platform_id));

  VLOG(1) << "Added an accelerator queue "
          << "[" << acc_conf.id() << "] "
          << "for platform: " << platform_id;
}

void PlatformManager::removeAcc(
    std::string requester,  // TODO: used to verify client ID
    std::string acc_id,
    std::string platform_id) 
{
  // lock all tables to guarantee exclusive access
  boost::lock_guard<PlatformManager> guard(*this);

  // check if acc of the same already exists
  if (acc_table.find(acc_id) == acc_table.end()) {
    return;
  }
  Platform_ptr platform = platform_table[platform_id];

  if (!platform) {
    throw std::runtime_error(
        "required platform does not exist");
  }

  // remove mappings of acc_id
  acc_table.erase(acc_id);

  // setup the task environment with ACC conf
  platform->removeQueue(acc_id);

  VLOG(1) << "Removed an accelerator queue "
          << "[" << acc_id << "] "
          << "for platform: " << platform_id;
}

// create a new platform
Platform_ptr PlatformManager::create(
    std::string path, 
    std::map<std::string, std::string> &conf_table) 
{
  if (path.compare("")==0) {
    Platform_ptr platform(new Platform(conf_table));
    return platform;
  }
  else {
    void* handle = dlopen(path.c_str(), RTLD_LAZY|RTLD_LOCAL);

    if (handle == NULL) {
      throw std::runtime_error(dlerror());
    }

    // reset errors
    dlerror();

    // load the symbols
    Platform* (*create_func)(std::map<std::string, std::string>&);
    void (*destroy_func)(Platform*);

    // read the custom constructor and destructor  
    create_func = (Platform* (*)(std::map<std::string, std::string>&))
                    dlsym(handle, "create");
    destroy_func = (void (*)(Platform*))dlsym(handle, "destroy");

    const char* error = dlerror();
    if (error) {
      throw std::runtime_error(error);
    }

    Platform_ptr platform(create_func(conf_table), destroy_func);

    return platform;
  }
}

void PlatformManager::removeShared(int64_t block_id)
{
  try {
    // TODO: need to investigate further
    for (auto it : platform_table) {
      it.second->remove(block_id);
    }
  }
  catch (std::runtime_error &e) {
    throw(e);
  }
}

std::vector<std::pair<std::string, std::string> > PlatformManager::getLabels()
{
  std::vector<std::pair<std::string, std::string> > ret;
  std::map<std::string, std::string>::iterator iter;
  for (iter = acc_table.begin();
       iter != acc_table.end();
       iter ++ )
  {
    ret.push_back(std::make_pair(iter->first, iter->second)); 
  }
  return ret;
}

} // namespace blaze

