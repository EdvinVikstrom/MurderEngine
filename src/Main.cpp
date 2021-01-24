/* ignore the mess just testing */

#include "engine/EngineBus.hpp"
#include "engine/MurderEngine.hpp"
#include "engine/audio/AudioSystem.hpp"
#include "engine/renderer/Renderer.hpp"
#include "engine/renderer/Shader.hpp"
#include "engine/scene/Camera.hpp"
#include "engine/scene/Mesh.hpp"
#include "engine/scene/SceneInfo.hpp"
#include "engine/surface/window/WindowSurface.hpp"
#include "engine/renderer/vulkan/Vulkan.hpp"
#include "engine/audio/portaudio/PortAudio.hpp"

#include "engine/format/shader/shader_format.hpp"

#include "engine/scene/Scene.hpp"
#include "lme/math/math.hpp"

#include <cfloat>
#include <lme/file.hpp>
#include <math.h>

class Sandbox : public me::Module {

public:

  Sandbox()
    : Module(me::MODULE_OTHER_TYPE, "sandbox")
  {
  }

protected:

  me::Scene* scene;

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

  me::Module* modules[3];
  modules[0] = new me::WindowSurface(surface_user_callbacks);
  modules[1] = new me::vulkan::Vulkan();
  modules[2] = new Sandbox();
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

  me::EngineBus engine_bus = {
    .module_count = engine_info.module_count,
    .modules = engine_info.modules
  };

  me::MurderEngine engine(engine_info, engine_bus);
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

  me::vector<me::Vertex> vertices;
  me::vector<me::Index> indices;
  vertices.push_back({{-0.5F, -0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {1.0F, 0.0F, 0.0F, 1.0F}});
  vertices.push_back({{0.5F, -0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {0.0F, 0.0F, 1.0F, 1.0F}});
  vertices.push_back({{0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {1.0F, 1.0F, 0.0F, 1.0F}});
  vertices.push_back({{-0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {0.0F, 1.0F, 0.0F, 1.0F}});
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(0);

  me::Mesh* test_mesh1 = new me::Mesh(vertices, indices);
  me::MeshRef* test_mesh1_ref = new me::MeshRef(test_mesh1, me::MESH_VISIBLE_FLAG);

  me::SceneInfo scene_info = {
    .type = me::SCENE_3D_VIEWPORT_TYPE
  };
  me::math::transform3f transform;
  me::Camera camera(me::PROJECTION_PERSPECTIVE_TYPE, me::math::radians(45.0F), 1.0F, 0.1F, 10.0F, transform);
  me::MemoryAlloc scene_alloc = module_info.alloc.child(4096);

  scene = new me::Scene(scene_info, scene_alloc, camera);




  renderer->register_mesh(test_mesh1_ref);

  me::Device device;
  me::Swapchain swapchain;
  me::Memory memory;
  me::RenderPass render_pass;
  me::Pipeline pipeline;
  me::Framebuffer framebuffer;
  me::UniformBuffer uniform_buffer;
  me::Descriptor descriptor;
  me::CommandBuffer command_buffer;

  me::Viewport viewport;
  viewport.location[0] = 0.0F;
  viewport.location[1] = 0.0F;
  viewport.size[0] = 1550.0F;
  viewport.size[1] = 770.0F;

  me::Scissor scissor;
  scissor.offset[0] = 0;
  scissor.offset[1] = 0;
  scissor.size[0] = 1550;
  scissor.size[1] = 770;

  me::RasterizerInfo rasterizer_info = {
    .front_face = me::FRONT_FACE_COUNTER_CLOCKWISE,
    .polygon_mode = me::POLYGON_MODE_FILL,
    .cull_mode = me::CULL_MODE_BACK
  };

  me::MultisamplingInfo multisampling_info = {
    .sample_count = me::SAMPLE_COUNT_1
  };

  me::vector<me::ShaderBinding> vertex_bindings = {
    {0, sizeof(me::Vertex)}
  };

  me::ShaderAttribute position_attrib = {"position", me::FORMAT_VECTOR3_32FLOAT, 0, 0, offsetof(me::Vertex, position)};
  me::ShaderAttribute normal_attrib = {"normal", me::FORMAT_VECTOR3_32FLOAT, 0, 1, offsetof(me::Vertex, normal)};
  me::ShaderAttribute tex_coord_attrib = {"tex_coord", me::FORMAT_VECTOR2_32FLOAT, 0, 2, offsetof(me::Vertex, tex_coord)};
  me::ShaderAttribute color_attrib = {"color", me::FORMAT_VECTOR4_32FLOAT, 0, 3, offsetof(me::Vertex, color)};
  me::vector<me::ShaderAttribute> vertex_attributes = {
    position_attrib,
    normal_attrib,
    tex_coord_attrib,
    color_attrib
  };

  me::ShaderInfo shader_info = {
    .vertex_bindings = vertex_bindings,
    .vertex_attributes = vertex_attributes,
    .topology = me::TOPOLOGY_TRIANGLE_LIST,
    .shaders = {vertex_shader, fragment_shader}
  };

  me::PipelineInfo pipeline_info = {
    .viewports = {viewport},
    .scissors = {scissor},
    .rasterizer_info = rasterizer_info,
    .multisampling_info = multisampling_info,
    .shader_info = shader_info
  };

  me::FramebufferInfo framebuffer_info;
  framebuffer_info.size[0] = 1550;
  framebuffer_info.size[1] = 770;
  framebuffer_info.layers = 1;
  framebuffer_info.clear_values[0] = 0.0F;
  framebuffer_info.clear_values[1] = 1.0F;
  framebuffer_info.clear_values[2] = 0.0F;
  framebuffer_info.clear_values[3] = 1.0F;

  me::UniformBufferInfo uniform_buffer_info = {
    .size = sizeof(me::SceneUniform),
    .data = const_cast<me::SceneUniform*>(&scene->get_uniform())
  };

  me::SurfaceInfo surface_info = {
    .surface = module_info.engine_bus->get_active_surface_module()
  };

  me::DeviceInfo device_info = {
  };

  me::SwapchainInfo swapchain_info = {
  };

  me::MemoryInfo memory_info = {
  };

  me::RenderPassInfo render_pass_info = {
  };


  renderer->init_engine(*module_info.engine_info, module_info.engine_bus->get_active_surface_module());
  renderer->setup_device(device_info, device);
  renderer->setup_surface(surface_info);
  renderer->setup_swapchain(swapchain_info, swapchain);
  renderer->setup_memory(memory_info, memory);
  renderer->setup_render_pass(render_pass_info, render_pass);
  renderer->setup_pipeline(pipeline_info, pipeline);
  renderer->setup_framebuffer(framebuffer_info, framebuffer);
  renderer->setup_uniform_buffer(uniform_buffer_info, uniform_buffer);

  me::DescriptorInfo descriptor_info = {
    .uniform_buffer = uniform_buffer,
    .offset = 0,
    .range = sizeof(me::SceneUniform)
  };

  renderer->setup_descriptor(descriptor_info, descriptor);

  me::CommandBufferInfo command_buffer_info = {
    .framebuffer = framebuffer,
    .descriptor = descriptor
  };
  
  renderer->setup_command_buffer(command_buffer_info, command_buffer);
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
