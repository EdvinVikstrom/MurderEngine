/* ignore the mess just testing */

#include "engine/MurderEngine.hpp"
#include "engine/audio/AudioSystem.hpp"
#include "engine/renderer/Shader.hpp"
#include "engine/surface/window/WindowSurface.hpp"
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

static int init_window_callback(me::Surface::Config &config)
{
  config.title = "A title";
  config.width = 1550;
  config.height = 770;
  return 0;
}

int main(int argc, char** argv)
{
  me::Surface::UserCallbacks surface_user_callbacks = {
    .init_surface = init_window_callback
  };
  me::Surface::Callbacks surface_callbacks = {
  };

  me::Module* modules[3];
  modules[0] = new Sandbox();
  modules[1] = new me::WindowSurface(surface_user_callbacks, surface_callbacks);
  modules[2] = new me::Vulkan(reinterpret_cast<me::Surface*>(modules[1]));
  //modules[1] = new me::PortAudio();

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

  me::vector<me::Vertex> vertices;
  float scale = 1.0F;
  vertices.push_back({{0.0F * scale, -0.5F * scale, 0.0F}, {0.0F * scale, -0.5F * scale, 0.0F}, {0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F}});
  vertices.push_back({{0.5F * scale, 0.5F * scale, 0.0F}, {0.5F * scale, 0.5F * scale, 0.0F}, {0.0F, 0.0F}, {1.0F, 0.0F, 1.0F, 1.0F}});
  vertices.push_back({{-0.5F * scale, 0.5F * scale, 0.0F}, {-0.5F * scale, 0.5F * scale, 0.0F}, {0.0F, 0.0F}, {1.0F, 1.0F, 0.0F, 1.0F}});
  me::Mesh* mesh = new me::Mesh("main", vertices);
  renderer->register_mesh(mesh);
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
