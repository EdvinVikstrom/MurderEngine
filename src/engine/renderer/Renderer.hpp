#ifndef ME_RENDERER_HPP
  #define ME_RENDERER_HPP

#include "Shader.hpp"

#include "../Module.hpp"

#include "../scene/Mesh.hpp"

namespace me {

  class Renderer : public Module {

  public:

    Renderer(const string &name)
      : Module(MODULE_RENDERER_TYPE, name)
    {
    }

    virtual int register_shader(Shader* shader) = 0;
    virtual int register_mesh(Mesh* mesh) = 0;

  };

}

#endif
