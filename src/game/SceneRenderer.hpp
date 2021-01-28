#ifndef SCENE_RENDERER_HPP
  #define SCENE_RENDERER_HPP

#include "../engine/Module.hpp"

class SceneRenderer : public me::Module {

public:

  SceneRenderer();

  int initialize(const me::ModuleInfo) override;
  int terminate(const me::ModuleInfo) override;
  int tick(const me::ModuleInfo) override;

};

#endif
