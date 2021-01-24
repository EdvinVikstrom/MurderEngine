#ifndef ME_RENDERER_HPP
  #define ME_RENDERER_HPP

#include "Shader.hpp"

#include "../Module.hpp"

#include "../scene/Mesh.hpp"

#include <lme/math/matrix.hpp>

namespace me {

  enum PolygonMode {
    POLYGON_MODE_FILL
  };
  
  enum CullMode {
    CULL_MODE_BACK,
    CULL_MODE_FRONT,
    CULL_MODE_FRONT_BACK
  };

  enum FrontFace {
    FRONT_FACE_CLOCKWISE,
    FRONT_FACE_COUNTER_CLOCKWISE
  };

  enum Topology {
    TOPOLOGY_TRIANGLE_LIST
  };

  enum SampleCount {
    SAMPLE_COUNT_1,
    SAMPLE_COUNT_2,
    SAMPLE_COUNT_4,
    SAMPLE_COUNT_8,
    SAMPLE_COUNT_16,
    SAMPLE_COUNT_32,
    SAMPLE_COUNT_64
  };

  enum Format {
    FORMAT_32FLOAT,
    FORMAT_VECTOR2_32FLOAT,
    FORMAT_VECTOR3_32FLOAT,
    FORMAT_VECTOR4_32FLOAT
  };


  typedef uint32_t Device;
  typedef uint32_t Memory;
  typedef uint32_t Swapchain;
  typedef uint32_t RenderPass;
  typedef uint32_t Pipeline;
  typedef uint32_t Framebuffer;
  typedef uint32_t UniformBuffer;
  typedef uint32_t Descriptor;
  typedef uint32_t CommandBuffer;

  struct Viewport {
    float location[2];
    float size[2];
  };

  struct Scissor {
    int32_t offset[2];
    uint32_t size[2];
  };

  struct ShaderBinding {
    uint32_t binding;
    uint32_t stride;
  };

  struct ShaderAttribute {
    const char name[255];
    Format format;
    uint32_t binding;
    uint32_t location;
    uint32_t offset;
  };


  struct DeviceInfo {
  };

  struct MemoryInfo {
  };

  struct SurfaceInfo {
    Surface* surface;
  };

  struct SwapchainInfo {
  };

  struct RenderPassInfo {
  };

  struct RasterizerInfo {
    FrontFace front_face;
    PolygonMode polygon_mode;
    CullMode cull_mode;
  };
  
  struct MultisamplingInfo {
    SampleCount sample_count;
  };

  struct ShaderInfo {
    vector<ShaderBinding> vertex_bindings;
    vector<ShaderAttribute> vertex_attributes;
    Topology topology;
    vector<Shader*> shaders;
  };

  struct PipelineInfo {
    vector<Viewport> viewports;
    vector<Scissor> scissors;
    RasterizerInfo rasterizer_info;
    MultisamplingInfo multisampling_info;
    ShaderInfo shader_info;
  };

  struct FramebufferInfo {
    uint32_t size[2];
    uint32_t offset[2];
    uint32_t layers;
    float clear_values[4];
  };

  struct UniformBufferInfo {
    uint32_t size;
    void* data;
  };

  struct DescriptorInfo {
    UniformBuffer uniform_buffer;
    uint32_t offset;
    uint32_t range;
  };

  struct CommandBufferInfo {
    Framebuffer framebuffer;
    Descriptor descriptor;
  };

  struct MeshInfo {
  };

  class Renderer : public Module {

  protected:

  public:

    Renderer(const string &name)
      : Module(MODULE_RENDERER_TYPE, name)
    {
    }

    virtual int init_engine(const EngineInfo &engine_info, Surface* surface) = 0;
    virtual int setup_device(const DeviceInfo &device_info, Device &device) = 0;
    virtual int setup_surface(const SurfaceInfo &surface_info) = 0;
    virtual int setup_swapchain(const SwapchainInfo &swapchain_info, Swapchain &swapchain) = 0;
    virtual int setup_memory(const MemoryInfo &memory_info, Memory &memory) = 0;
    virtual int setup_render_pass(const RenderPassInfo &render_pass_info, RenderPass &render_pass) = 0;
    virtual int setup_pipeline(const PipelineInfo &pipeline_info, Pipeline &pipeline) = 0;
    virtual int setup_framebuffer(const FramebufferInfo &framebuffer_info, Framebuffer &framebuffer) = 0;
    virtual int setup_uniform_buffer(const UniformBufferInfo &uniform_buffer_info, UniformBuffer &uniform_buffer) = 0;
    virtual int setup_descriptor(const DescriptorInfo &descriptor_info, Descriptor &descriptor) = 0;
    virtual int setup_command_buffer(const CommandBufferInfo &command_buffer_info, CommandBuffer &command_buffer) = 0;

    virtual int register_mesh(MeshRef* mesh) = 0;

  };

}

#endif
