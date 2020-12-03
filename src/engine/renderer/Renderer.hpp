#ifndef ME_RENDERER_HPP
  #define ME_RENDERER_HPP

#include "../Module.hpp"

#include "../scene/Mesh.hpp"

namespace me {

  class Renderer : public Module {

  public:

    Renderer(const MurderEngine* engine, const std::string &name)
      : Module(engine, Module::RENDERER, name)
    {
    }

  };

}

#endif
