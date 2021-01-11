#ifndef ME_ENGINE_BUS_HPP
  #define ME_ENGINE_BUS_HPP

#include <lme/string.hpp>

namespace me {

  class EngineBus {

  protected:

    const uint32_t module_count;
    class Module** modules;

  public:

    EngineBus(const uint32_t module_count, class Module** modules);

    class Module** begin() const;
    class Module** end() const;

    class Module* get_module(const string &name) const;
    class Module* get_module(const uint32_t module_type) const;
    class Renderer* get_active_renderer_module() const;
    class AudioSystem* get_active_audio_module() const;

  };

}

#endif
