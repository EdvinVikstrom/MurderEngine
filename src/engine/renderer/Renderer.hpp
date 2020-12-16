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

    virtual int compile_shader(Shader* shader) const = 0;

  };

}

#endif
