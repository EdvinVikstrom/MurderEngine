#include "engine/MurderEngine.hpp"
#include "engine/renderer/Shader.hpp"
#include "engine/surface/VulkanSurface.hpp"
#include "engine/surface/window/VKWindowSurface.hpp"
#include "engine/renderer/vulkan/Vulkan.hpp"
#include "engine/audio/portaudio/PortAudio.hpp"

#include "engine/format/shader/shader_format.hpp"

#include <lme/file.hpp>

class Sandbox : public me::Module {

public:

  Sandbox()
    : Module(me::MODULE_OTHER_TYPE, "sandbox")
  {
  }

protected:

  int initialize(const me::ModuleInfo) override;
  int terminate(const me::ModuleInfo) override;
  int tick(const me::ModuleInfo) override;

};

int main(int argc, char** argv)
{
  me::Module* modules[3];
  modules[0] = new Sandbox();
  modules[1] = new me::VKWindowSurface();
  modules[2] = new me::Vulkan(*(((me::VulkanSurface*) modules[1])));

  me::ApplicationInfo app_info = {
    .name = "Sandbox",
    .version = 1
  };

  me::EngineInfo engine_info = {
    .application_info = app_info,
    .module_count = 3,
    .modules = modules
  };

  me::MurderEngine engine(engine_info);
  engine.initialize(argc, argv);
  return engine.terminate();
}


int Sandbox::initialize(const me::ModuleInfo module_info)
{
  me::Renderer* renderer = module_info.engine_bus->get_active_renderer_module();

  size_t vert_size, frag_size;
  char *vert_data, *frag_data;

  me::File vertex_shader_file("src/res/vert.spv");
  me::File fragment_shader_file("src/res/frag.spv");

  me::File::read(vertex_shader_file, vert_size, vert_data);
  me::File::read(fragment_shader_file, frag_size, frag_data);

  me::Shader* vertex_shader = new me::Shader("vertex", me::SHADER_VERTEX, vert_size, vert_data, { .entry_point = "main" });
  me::Shader* fragment_shader = new me::Shader("fragment", me::SHADER_FRAGMENT, frag_size, frag_data, { .entry_point = "main" });

  renderer->register_shader(vertex_shader);
  renderer->register_shader(fragment_shader);

  return 0;
}

int Sandbox::terminate(const me::ModuleInfo module_info)
{
  return 0;
}

int Sandbox::tick(const me::ModuleInfo module_info)
{
  return 0;
}
