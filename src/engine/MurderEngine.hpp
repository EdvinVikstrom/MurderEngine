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

    allocator alloc;

  public:

    explicit MurderEngine(const EngineInfo &engine_info, const EngineBus &engine_bus);

    int initialize(int argc, char** argv);
    int terminate();

  protected:

    int translate_semaphore(const Semaphore &semaphore, const string_view &module);

    int init_modules();
    int tick_modules();
    int terminate_modules();

  };

}

#endif
