#include "../engine/renderer/vulkan/Vulkan.hpp"
#include "../engine/surface/window/WindowSurface.hpp"
#include "Game.hpp"
#include "SceneRenderer.hpp"

#include "../engine/MurderEngine.hpp"

static int callback_init_surface(me::SurfaceModule::Config &config)
{
  config.title = "game";
  config.width = 1550;
  config.height = 770;
  return 0;
}

int main(int argc, char** argv)
{
  me::SurfaceModule::UserCallbacks surface_callbacks;
  surface_callbacks.init_surface = callback_init_surface;

  me::Module* modules[4];
  modules[0] = new Game;
  modules[1] = new me::WindowSurface(surface_callbacks);
  modules[2] = new me::Vulkan;
  modules[3] = new SceneRenderer;

  me::ApplicationInfo app_info = {};
  app_info.name = "Game";
  app_info.version = 1;

  me::EngineInfo engine_info = {};
  engine_info.application_info = app_info;

  me::EngineBus engine_bus = {};
  engine_bus.module_count = 4;
  engine_bus.modules = modules;

  me::MurderEngine engine(engine_info, engine_bus);
  engine.initialize(argc, argv);
  return engine.terminate();
}
