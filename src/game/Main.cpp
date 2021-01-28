#include "../engine/renderer/vulkan/Vulkan.hpp"
#include "Game.hpp"
#include "SceneRenderer.hpp"

#include "../engine/MurderEngine.hpp"

int main(int argc, char** argv)
{
  me::Module* modules[3];
  modules[0] = new Game;
  modules[1] = new me::Vulkan;
  modules[2] = new SceneRenderer;

  me::ApplicationInfo app_info = {};
  app_info.name = "Game";
  app_info.version = 1;

  me::EngineInfo engine_info = {};
  engine_info.application_info = app_info;

  me::EngineBus engine_bus = {};
  engine_bus.module_count = 3;
  engine_bus.modules = modules;

  me::MurderEngine engine(engine_info, engine_bus);
  engine.initialize(argc, argv);
  return engine.terminate();
}
