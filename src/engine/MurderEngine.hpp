#ifndef MURDERENGINE_HPP
  #define MURDERENGINE_HPP

#include "EngineInfo.hpp"

#include "Logger.hpp"
#include "Module.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class MurderEngine {

  private:

    Logger logger;

    bool running = false;

  protected:

    const EngineInfo engine_info;
    const EngineBus engine_bus;

  public:

    explicit MurderEngine(const EngineInfo &engine_info);

    int initialize(int argc, char** argv);
    int terminate();

  protected:

    int translate_semaphore(const Semaphore &semaphore);

    int init_modules(Semaphore &semaphore);
    int tick_modules(Semaphore &semaphore);
    int terminate_modules(Semaphore &semaphore);

  };

}

#endif
