#include "SceneRenderer.hpp"

SceneRenderer::SceneRenderer()
  : Module(me::MODULE_LOGIC_TYPE, "scene_renderer")
{
}

int SceneRenderer::initialize(const me::ModuleInfo)
{
  return 0;
}

int SceneRenderer::terminate(const me::ModuleInfo)
{
  return 0;
}

int SceneRenderer::tick(const me::ModuleInfo)
{
  return 0;
}
