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

  engine.load_module(sandbox->surface);
  engine.load_module(sandbox->renderer);
  engine.load_module(sandbox->audio_system);

  engine.load_module(sandbox);

  engine.initialize_loop();
  return engine.terminate();
}


int Sandbox::initialize()
{
  size_t size;
  char* data;

  me::File shader_file("src/res/shader_test.vert");
  me::File::read(shader_file, size, data);

  me::ByteBuffer buffer(size, data);
  me::format::Shader_Result result;
  me::format::shader_read(shader_file.get_path(), buffer, result);

  me::Shader* vertex_shader = result.shaders.at(0);

  renderer->compile_shader(vertex_shader);

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
