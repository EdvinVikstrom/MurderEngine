#ifndef ME_RENDERER_HPP
  #define ME_RENDERER_HPP

#include "Shader.hpp"

#include "../Module.hpp"

#include "../scene/Mesh.hpp"

namespace me {

  class Renderer : public Module {

  public:

    Renderer(const MurderEngine* engine, const string &name)
      : Module(engine, Module::RENDERER, name)
    {
    }

    virtual int queue_shader(Shader* shader) const = 0;
    virtual int compile_shader(Shader* shader) const = 0;
    virtual int register_shader(Shader* shader) const = 0;

  };

}

#endif
