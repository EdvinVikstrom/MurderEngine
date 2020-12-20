#ifndef ME_SURFACE_HPP
  #define ME_SURFACE_HPP

#include "RenderLayer.hpp"

#include "../Module.hpp"

#include <lme/vector.hpp>
#include <lme/string.hpp>

namespace me {

  class Surface : public Module {

  public:

    struct Config {
      uint32_t fps;
    };

  protected:

    mutable Config config;

  public:

    explicit Surface(const MurderEngine* engine, const string &name, Config config)
      : Module(engine, Module::SURFACE, name)
    {
      this->config = config;
    }

    Config& get_config() const
    {
      return config;
    }

    virtual int get_size(uint32_t &width, uint32_t &height) const = 0;

    virtual int register_layer(RenderLayer* layer) const = 0;

  };

}

#endif
