#ifndef MURDERENGINE_HPP
  #define MURDERENGINE_HPP

#ifndef ME_ENGINE_NAME
  #define ME_ENGINE_NAME 		"MurderEngine"
#endif
#ifndef ME_ENGINE_VERSION
  #define ME_ENGINE_VERSION		"202012"
  #define ME_ENGINE_VERSION_MAJOR	0
  #define ME_ENGINE_VERSION_MINOR	1
  #define ME_ENGINE_VERSION_PATCH	0
#endif


#include "Logger.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class Module;


  /* application configure */
  struct AppConfig {
    const char* name;
    const uint32_t version;
  };


  class MurderEngine {

  private:

    const AppConfig app_config;
    Logger logger;

    bool running = false;
    vector<Module*> modules;

  public:

    explicit MurderEngine(const AppConfig &config);

    const AppConfig& get_app_config() const;
    const Logger& get_logger() const;

    int initialize(int argc, char** argv);
    int initialize_loop();
    int terminate();

    int load_module(Module* module);
    Module* get_module(const string &id) const;

    int print_module_exception(const Module &module, const exception &except);

  };

}

#endif
