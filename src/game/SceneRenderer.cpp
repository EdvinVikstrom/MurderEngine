#include "SceneRenderer.hpp"
#include "../engine/renderer/Renderer.hpp"

#include <lme/file.hpp>

static int create_queue(me::RendererModule* renderer, me::Device device, me::QueueType queue_type, me::Queue &queue)
{
  me::QueueCreateInfo queue_create_info = {};
  queue_create_info.type = me::STRUCTURE_TYPE_QUEUE_CREATE_INFO;
  queue_create_info.next = nullptr;
  queue_create_info.device = device;
  queue_create_info.queue_type = queue_type;
  renderer->create_queue(queue_create_info, queue);
  return 0;
}


SceneRenderer::SceneRenderer()
  : Module(me::MODULE_LOGIC_TYPE, "scene_renderer")
{
  mesh = new me::Mesh;
  mesh->vertices.push_back({{-0.5F, -0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {0.0F, 1.0F, 1.0F, 1.0F}});
  mesh->vertices.push_back({{0.5F, -0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {1.0F, 1.0F, 0.0F, 1.0F}});
  mesh->vertices.push_back({{0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {0.0F, 0.0F, 1.0F, 1.0F}});
  mesh->vertices.push_back({{-0.5F, 0.5F, 0.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}, {1.0F, 1.0F, 0.0F, 1.0F}});

  mesh->indices.push_back({0});
  mesh->indices.push_back({1});
  mesh->indices.push_back({2});
  mesh->indices.push_back({2});
  mesh->indices.push_back({3});
  mesh->indices.push_back({0});
}

int SceneRenderer::initialize(const me::ModuleInfo module_info)
{
  me::SurfaceModule* surface_module = module_info.engine_bus->get_active_surface_module();
  me::RendererModule* renderer = module_info.engine_bus->get_active_renderer_module();

  uint32_t surface_width, surface_height;
  surface_module->get_framebuffer_size(surface_width, surface_height);

  uint32_t extension_count;
  const char** extensions = surface_module->vk_get_required_surface_extensions(extension_count);

  me::EngineInitInfo engine_init_info = {};
  engine_init_info.engine_info = module_info.engine_info;
  engine_init_info.alloc = me::allocator();
  engine_init_info.extension_count = extension_count;
  engine_init_info.extensions = extensions;
  engine_init_info.debug = false;
  renderer->init_engine(engine_init_info);

  /* getting all the physical devices */
  uint32_t physical_device_count;
  renderer->enumerate_physical_devices(physical_device_count, nullptr);
  me::PhysicalDevice physical_devices[physical_device_count];
  renderer->enumerate_physical_devices(physical_device_count, physical_devices);
  physical_device = physical_devices[0];

  /* creating a surface */
  me::SurfaceCreateInfo surface_create_info = {};
  surface_create_info.type = me::STRUCTURE_TYPE_SURFACE_CREATE_INFO;
  surface_create_info.next = nullptr;
  surface_create_info.physical_device = physical_device;
  surface_create_info.surface_module = surface_module;
  renderer->create_surface(surface_create_info, surface);

  /* creating a device */
  me::DeviceCreateInfo device_create_info = {};
  device_create_info.type = me::STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.next = nullptr;
  device_create_info.surface = surface;
  device_create_info.physical_device_count = 1;
  device_create_info.physical_devices = &physical_device;
  renderer->create_device(device_create_info, device);

  /* creating queues */
  create_queue(renderer, device, me::QUEUE_COMPUTE_TYPE, compute_queue);
  create_queue(renderer, device, me::QUEUE_GRAPHICS_TYPE, graphics_queue);
  create_queue(renderer, device, me::QUEUE_PRESENT_TYPE, present_queue);
  create_queue(renderer, device, me::QUEUE_GRAPHICS_TYPE, transfer_queue);

  /* creating a swapchain */
  me::SwapchainCreateInfo swapchain_create_info = {};
  swapchain_create_info.type = me::STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO;
  swapchain_create_info.next = nullptr;
  swapchain_create_info.device = device;
  swapchain_create_info.surface = surface;
  renderer->create_swapchain(swapchain_create_info, swapchain);

  /* creating swapchain images */
  uint32_t swapchain_image_count;
  renderer->get_swapchain_image_count(device, swapchain, swapchain_image_count);
  swapchain_images.resize(swapchain_image_count);

  me::SwapchainImageCreateInfo swapchain_image_create_info = {};
  swapchain_image_create_info.type = me::STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO;
  swapchain_image_create_info.next = nullptr;
  swapchain_image_create_info.device = device;
  swapchain_image_create_info.swapchain = swapchain;
  renderer->create_swapchain_images(swapchain_image_create_info, swapchain_image_count, swapchain_images.data());

  /* creating frames */
  frames.resize(FRAME_COUNT);
  me::FrameCreateInfo frame_create_info = {};
  frame_create_info.type = me::STRUCTURE_TYPE_FRAME_CREATE_INFO;
  frame_create_info.next = nullptr;
  frame_create_info.device = device;
  renderer->create_frames(frame_create_info, FRAME_COUNT, frames.data());

  /* creating render pass */
  me::RenderPassCreateInfo render_pass_create_info = {};
  render_pass_create_info.type = me::STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.next = nullptr;
  render_pass_create_info.device = device;
  render_pass_create_info.swapchain = swapchain;
  renderer->create_render_pass(render_pass_create_info, render_pass);

  /* creating pipeline */
  me::RasterizerCreateInfo rasterizer_create_info = {};
  rasterizer_create_info.type = me::STRUCTURE_TYPE_RASTERIZER_CREATE_INFO;
  rasterizer_create_info.next = nullptr;
  rasterizer_create_info.polygon_mode = me::POLYGON_MODE_FILL;
  rasterizer_create_info.front_face = me::FRONT_FACE_CLOCKWISE;
  rasterizer_create_info.cull_mode = me::CULL_MODE_BACK;

  me::MultisamplingCreateInfo multisampling_create_info = {};
  multisampling_create_info.type = me::STRUCTURE_TYPE_MULTISAMPLING_CREATE_INFO;
  multisampling_create_info.next = nullptr;
  multisampling_create_info.samples = me::SAMPLE_COUNT_1;

  me::ShaderBindingInfo vertex_binding = {
    .binding = 0,
    .stride = 0
  };

  static constexpr uint32_t vertex_attribute_count = 4;
  me::ShaderAttributeInfo vertex_attributes[vertex_attribute_count];
  vertex_attributes[0].name = "position";
  vertex_attributes[0].binding = 0;
  vertex_attributes[0].location = 0;
  vertex_attributes[0].offset = offsetof(me::Vertex, position);
  vertex_attributes[0].format = me::FORMAT_VECTOR3_32FLOAT;

  vertex_attributes[1].name = "normal";
  vertex_attributes[1].binding = 0;
  vertex_attributes[1].location = 1;
  vertex_attributes[1].offset = offsetof(me::Vertex, normal);
  vertex_attributes[1].format = me::FORMAT_VECTOR3_32FLOAT;

  vertex_attributes[2].name = "tex_coord";
  vertex_attributes[2].binding = 0;
  vertex_attributes[2].location = 2;
  vertex_attributes[2].offset = offsetof(me::Vertex, tex_coord);
  vertex_attributes[2].format = me::FORMAT_VECTOR2_32FLOAT;

  vertex_attributes[3].name = "color";
  vertex_attributes[3].binding = 0;
  vertex_attributes[3].location = 3;
  vertex_attributes[3].offset = offsetof(me::Vertex, color);
  vertex_attributes[3].format = me::FORMAT_VECTOR4_32FLOAT;

  static constexpr uint32_t shader_count = 2;
  size_t vert_shader_len;
  char* vert_shader_data;
  me::File::read(me::File("src/res/vert.spv"), vert_shader_len, vert_shader_data);

  size_t frag_shader_len;
  char* frag_shader_data;
  me::File::read(me::File("src/res/frag.spv"), frag_shader_len, frag_shader_data);

  me::Shader::Config shader_config = {
    .entry_point = "main"
  };
  me::Shader* shaders[shader_count];
  shaders[0] = new me::Shader("vert", me::SHADER_VERTEX, vert_shader_len, vert_shader_data, shader_config);
  shaders[1] = new me::Shader("frag", me::SHADER_FRAGMENT, frag_shader_len, frag_shader_data, shader_config);

  me::ShaderCreateInfo shader_create_info = {};
  shader_create_info.type = me::STRUCTURE_TYPE_SHADER_CREATE_INFO;
  shader_create_info.next = nullptr;
  shader_create_info.vertex_binding_count = 1;
  shader_create_info.vertex_bindings = &vertex_binding;
  shader_create_info.vertex_attribute_count = vertex_attribute_count;
  shader_create_info.vertex_attributes = vertex_attributes;
  shader_create_info.shader_count = shader_count;
  shader_create_info.shaders = shaders;
  shader_create_info.topology = me::TOPOLOGY_TRIANGLE_LIST;

  uint32_t viewport_count = 1;
  me::ViewportInfo viewports[viewport_count];
  viewports[0].location = {0.0F, 0.0F};
  viewports[0].size = {(float) surface_width, (float) surface_height};

  uint32_t scissor_count = 1;
  me::ScissorInfo scissors[scissor_count];
  scissors[0].offset = {0, 0};
  scissors[0].size = {surface_width, surface_height};

  me::PipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.type = me::STRUCTURE_TYPE_PIPELINE_CREATE_INFO;
  pipeline_create_info.next = nullptr;
  pipeline_create_info.device = device;
  pipeline_create_info.render_pass = render_pass;
  pipeline_create_info.viewport_count = viewport_count;
  pipeline_create_info.viewports = viewports;
  pipeline_create_info.scissor_count = scissor_count;
  pipeline_create_info.scissors = scissors;
  pipeline_create_info.rasterizer_create_info = &rasterizer_create_info;
  pipeline_create_info.multisampling_create_info = &multisampling_create_info;
  pipeline_create_info.shader_create_info = &shader_create_info;
  renderer->create_pipeline(pipeline_create_info, pipeline);

  /* creating framebuffers */
  framebuffers.resize(swapchain_images.size());
  for (uint32_t i = 0; i < swapchain_images.size(); i++)
  {
    me::FramebufferCreateInfo framebuffer_create_info = {};
    framebuffer_create_info.type = me::STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.next = nullptr;
    framebuffer_create_info.device = device;
    framebuffer_create_info.render_pass = render_pass;
    framebuffer_create_info.image = swapchain_images[i];
    framebuffer_create_info.offset = {0, 0};
    framebuffer_create_info.size = {surface_width, surface_height};
    renderer->create_framebuffer(framebuffer_create_info, framebuffers[i]);
  }

  /* creating a descriptor pool */
  me::DescriptorPoolCreateInfo descriptor_pool_create_info = {};
  descriptor_pool_create_info.type = me::STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.next = nullptr;
  descriptor_pool_create_info.device = device;
  descriptor_pool_create_info.descriptor_type = me::DESCRIPTOR_TYPE_UNIFORM;
  descriptor_pool_create_info.descriptor_count = swapchain_images.size();
  renderer->create_descriptor_pool(descriptor_pool_create_info, descriptor_pool);

  /* creating uniform buffers */
  uniform_buffers.resize(swapchain_images.size());
  for (uint32_t i = 0; i < swapchain_images.size(); i++)
  {
    me::BufferCreateInfo buffer_create_info = {};
    buffer_create_info.type = me::STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.next = nullptr;
    buffer_create_info.physical_device = physical_device;
    buffer_create_info.device = device;
    buffer_create_info.usage = me::BUFFER_USAGE_UNIFORM_BUFFER;
    buffer_create_info.write_method = me::BUFFER_WRITE_METHOD_STANDARD;
    buffer_create_info.size = sizeof(UniformBufferObject);
    renderer->create_buffer(buffer_create_info, uniform_buffers[i]);
  }

  /* creating descriptors */
  descriptors.resize(swapchain_images.size());
  me::DescriptorCreateInfo descriptor_create_info = {};
  descriptor_create_info.type = me::STRUCTURE_TYPE_DESCRIPTOR_CREATE_INFO;
  descriptor_create_info.next = nullptr;
  descriptor_create_info.device = device;
  descriptor_create_info.pipeline = pipeline;
  descriptor_create_info.descriptor_pool = descriptor_pool;
  descriptor_create_info.descriptor_type = me::DESCRIPTOR_TYPE_UNIFORM;
  descriptor_create_info.buffer_count = uniform_buffers.size();
  descriptor_create_info.buffers = uniform_buffers.data();
  descriptor_create_info.offset = 0;
  descriptor_create_info.range = sizeof(UniformBufferObject);
  renderer->create_descriptors(descriptor_create_info, descriptors.size(), descriptors.data());

  /* creating command pools */
  me::CommandPoolCreateInfo graphics_command_pool_create_info = {};
  graphics_command_pool_create_info.type = me::STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  graphics_command_pool_create_info.next = nullptr;
  graphics_command_pool_create_info.device = device;
  graphics_command_pool_create_info.queue = graphics_queue;
  renderer->create_command_pool(graphics_command_pool_create_info, graphics_command_pool);

  me::CommandPoolCreateInfo transfer_command_pool_create_info = {};
  transfer_command_pool_create_info.type = me::STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  transfer_command_pool_create_info.next = nullptr;
  transfer_command_pool_create_info.device = device;
  transfer_command_pool_create_info.queue = transfer_queue;
  renderer->create_command_pool(transfer_command_pool_create_info, transfer_command_pool);

  /* creating command buffers */
  me::CommandBufferCreateInfo command_buffer_create_info = {};
  command_buffer_create_info.type = me::STRUCTURE_TYPE_COMMAND_BUFFER_CREATE_INFO;
  command_buffer_create_info.next = nullptr;
  command_buffer_create_info.device = device;
  command_buffer_create_info.command_pool = graphics_command_pool;
  command_buffer_create_info.usage = me::COMMAND_BUFFER_USAGE_RENDERING;
  draw_command_buffers.resize(swapchain_images.size());
  renderer->create_command_buffers(command_buffer_create_info, swapchain_images.size(), draw_command_buffers.data());

  /* creating mesh */
  me::SetupMeshInfo setup_mesh_info = {};
  setup_mesh_info.physical_device = physical_device;
  setup_mesh_info.device = device;
  setup_mesh_info.transfer_queue = transfer_queue;
  setup_mesh_info.transfer_pool = transfer_command_pool;
  renderer->setup_mesh(setup_mesh_info, mesh);

  for (uint32_t i = 0; i < draw_command_buffers.size(); i++)
  {
    me::CommandBuffer command_buffer = draw_command_buffers[i];
    renderer->cmd_record_start({}, command_buffer);

    me::CmdBeginRenderPassInfo cmd_begin_render_pass_info = {};
    cmd_begin_render_pass_info.swapchain = swapchain;
    cmd_begin_render_pass_info.render_pass = render_pass;
    cmd_begin_render_pass_info.framebuffer = framebuffers[i];
    cmd_begin_render_pass_info.clear_values[0] = 0.0F;
    cmd_begin_render_pass_info.clear_values[1] = 0.0F;
    cmd_begin_render_pass_info.clear_values[2] = 0.0F;
    cmd_begin_render_pass_info.clear_values[3] = 1.0F;
    renderer->cmd_begin_render_pass(cmd_begin_render_pass_info, command_buffer);

    me::CmdBindDescriptorsInfo cmd_bind_descriptors_info = {};
    cmd_bind_descriptors_info.pipeline = pipeline;
    cmd_bind_descriptors_info.descriptor_count = 1;
    cmd_bind_descriptors_info.descriptors = &descriptors[i];
    renderer->cmd_bind_descriptors(cmd_bind_descriptors_info, command_buffer);

    me::CmdDrawMeshesInfo cmd_draw_meshes_info = {};
    cmd_draw_meshes_info.pipeline = pipeline;
    cmd_draw_meshes_info.mesh_count = 1;
    cmd_draw_meshes_info.meshes = &mesh;
    renderer->cmd_draw_meshes(cmd_draw_meshes_info, command_buffer);

    renderer->cmd_end_render_pass(command_buffer);

    renderer->cmd_record_stop({}, command_buffer);
  }
  return 0;
}

int SceneRenderer::terminate(const me::ModuleInfo module_info)
{
  me::RendererModule* renderer = module_info.engine_bus->get_active_renderer_module();
  me::CommandBufferCleanupInfo command_buffer_cleanup_info = {};
  command_buffer_cleanup_info.device = device;
  command_buffer_cleanup_info.command_pool = graphics_command_pool;
  renderer->cleanup_command_buffers(command_buffer_cleanup_info, draw_command_buffers.size(), draw_command_buffers.data());

  me::CommandPoolCleanupInfo command_pool_cleanup_info = {};
  command_pool_cleanup_info.device = device;
  renderer->cleanup_command_pool(command_pool_cleanup_info, graphics_command_pool);

  me::DescriptorCleanupInfo descriptor_cleanup_info = {};
  descriptor_cleanup_info.device = device;
  descriptor_cleanup_info.descriptor_pool = descriptor_pool;
  renderer->cleanup_descriptors(descriptor_cleanup_info, descriptors.size(), descriptors.data());

  me::DescriptorPoolCleanupInfo descriptor_pool_cleanup_info = {};
  descriptor_pool_cleanup_info.device = device;
  renderer->cleanup_descriptor_pool(descriptor_pool_cleanup_info, descriptor_pool);

  me::FramebufferCleanupInfo framebuffer_cleanup_info = {};
  framebuffer_cleanup_info.device = device;
  for (me::Framebuffer &framebuffer : framebuffers)
    renderer->cleanup_framebuffer(framebuffer_cleanup_info, framebuffer);

  me::PipelineCleanupInfo pipeline_cleanup_info = {};
  pipeline_cleanup_info.device = device;
  renderer->cleanup_pipeline(pipeline_cleanup_info, pipeline);

  me::RenderPassCleanupInfo render_pass_cleanup_info = {};
  render_pass_cleanup_info.device = device;
  renderer->cleanup_render_pass(render_pass_cleanup_info, render_pass);

  me::FrameCleanupInfo frame_cleanup_info = {};
  frame_cleanup_info.device = device;
  renderer->cleanup_frames(frame_cleanup_info, frames.size(), frames.data());

  me::SwapchainImageCleanupInfo swapchain_image_cleanup_info = {};
  swapchain_image_cleanup_info.device = device;
  renderer->cleanup_swapchain_images(swapchain_image_cleanup_info, swapchain_images.size(), swapchain_images.data());

  me::SwapchainCleanupInfo swapchain_cleanup_info = {};
  swapchain_cleanup_info.device = device;
  renderer->cleanup_swapchain(swapchain_cleanup_info, swapchain);

  me::DeviceCleanupInfo device_cleanup_info = {};
  renderer->cleanup_device(device_cleanup_info, device);

  me::SurfaceCleanupInfo surface_cleanup_info = {};
  surface_cleanup_info.physical_device = physical_device;
  renderer->cleanup_surface(surface_cleanup_info, surface);

  renderer->terminate_engine();
  return 0;
}

int SceneRenderer::tick(const me::ModuleInfo module_info)
{
  me::RendererModule* renderer = module_info.engine_bus->get_active_renderer_module();

  /* prepare */
  me::FramePrepareInfo frame_prepare_info = {};
  frame_prepare_info.device = device;
  frame_prepare_info.swapchain = swapchain;
  frame_prepare_info.frame = frames[frame_index];

  me::FramePrepared frame_prepared;
  renderer->frame_prepare(frame_prepare_info, frame_prepared);

  uint32_t image_index;
  renderer->frame_prepared_get_image_index(frame_prepared, image_index);

  /* render */
  me::FrameRenderInfo frame_render_info = {};
  frame_render_info.device = device;
  frame_render_info.queue = graphics_queue;
  frame_render_info.prepared = frame_prepared;
  frame_render_info.image = swapchain_images[image_index];
  frame_render_info.frame = frames[frame_index];
  frame_render_info.image_index = image_index;
  frame_render_info.frame_index = frame_index;
  frame_render_info.command_buffer_count = draw_command_buffers.size();
  frame_render_info.command_buffers = draw_command_buffers.data();

  me::FrameRendered frame_rendered;
  renderer->frame_render(frame_render_info, frame_rendered);

  /* present */
  me::FramePresentInfo frame_present_info = {};
  frame_present_info.device = device;
  frame_present_info.queue = present_queue;
  frame_present_info.rendered = frame_rendered;
  frame_present_info.image = swapchain_images[image_index];
  frame_present_info.frame = frames[frame_index];
  frame_present_info.image_index = image_index;
  frame_present_info.frame_index = frame_index;
  frame_present_info.swapchain_count = 1;
  frame_present_info.swapchains = &swapchain;

  me::FramePresented frame_presented;
  renderer->frame_present(frame_present_info, frame_presented);

  frame_index++;
  if (frame_index >= FRAME_COUNT)
    frame_index = 0;
  return 0;
}
