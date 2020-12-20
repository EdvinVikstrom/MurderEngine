#include "engine/MurderEngine.hpp"
#include "engine/surface/VulkanSurface.hpp"
#include "engine/surface/window/VKWindowSurface.hpp"
#include "engine/renderer/vulkan/Vulkan.hpp"
#include "engine/audio/portaudio/PortAudio.hpp"

#include "engine/format/shader/shader_format.hpp"

#include <lme/file.hpp>

class Sandbox : public me::Module {

public:

  me::VulkanSurface* surface;
  me::Renderer* renderer;
  me::AudioSystem* audio_system;

public:

  Sandbox(const me::MurderEngine* engine)
    : Module(engine, me::Module::OTHER, "sandbox")
  {
  }

protected:

  int initialize() override;
  int terminate() override;
  int tick(const Context context) override;

};

int main(int argc, char** argv)
{
  me::AppConfig app_config = {
    .name = "Sandbox",
    .version = 1
  };

  me::MurderEngine engine(app_config);
  engine.initialize(argc, argv);

  Sandbox* sandbox = new Sandbox(&engine);
  sandbox->surface = new me::VKWindowSurface(&engine, { .fps = 24 });
  sandbox->renderer = new me::Vulkan(&engine, *sandbox->surface);
  sandbox->audio_system = new me::PortAudio(&engine);

  engine.load_module(sandbox);
  engine.load_module(sandbox->surface);
  engine.load_module(sandbox->renderer);
  engine.load_module(sandbox->audio_system);


  engine.initialize_loop();
  return engine.terminate();
}


int Sandbox::initialize()
{
  size_t size;
  char* data;

  me::File vertex_shader_file("src/res/shader_test.vert");
  me::File fragment_shader_file("src/res/shader_test.frag");
  me::File::read(vertex_shader_file, size, data);
  me::File::read(fragment_shader_file, size, data);

  me::ByteBuffer vertex_buffer(size, data);
  me::format::Shader_Result vertex_result;
  me::format::shader_read(vertex_shader_file.get_path(), vertex_buffer, vertex_result);

  me::ByteBuffer fragment_buffer(size, data);
  me::format::Shader_Result fragment_result;
  me::format::shader_read(fragment_shader_file.get_path(), fragment_buffer, fragment_result);


  me::Shader* vertex_shader = vertex_result.shaders.at(0);
  me::Shader* fragment_shader = fragment_result.shaders.at(0);

  renderer->queue_shader(vertex_shader);
  renderer->queue_shader(fragment_shader);

  return 0;
}

int Sandbox::terminate()
{
  return 0;
}

int Sandbox::tick(const Context context)
{
  return 0;
}
