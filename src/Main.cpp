#include <iostream>

#include "engine/MurderEngine.hpp"
#include "engine/surface/window/Window.hpp"
#include "engine/renderer/vulkan/Vulkan.hpp"

int main(int argc, char** argv)
{
  me::AppConfig app_config = {
    .name = "Sandbox",
    .version = 1
  };

  me::MurderEngine engine(app_config);
  engine.initialize(argc, argv);

  me::Surface* surface = new me::Window(&engine);
  me::Renderer* renderer = new me::Vulkan(&engine, *surface);

  engine.load_module(surface);
  engine.load_module(renderer);

  return engine.initialize_loop();
}
