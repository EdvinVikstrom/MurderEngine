#ifndef SCENE_RENDERER_HPP
  #define SCENE_RENDERER_HPP

#include "../engine/Module.hpp"
#include "../engine/renderer/Renderer.hpp"

#include <lme/vector.hpp>
#include <lme/math/matrix.hpp>

class SceneRenderer : public me::Module {

protected:

  struct UniformBufferObject {
    me::math::mat4f view;
    me::math::mat4f model;
    me::math::mat4f projection;
  };

  me::PhysicalDevice physical_device;
  me::Surface surface;
  me::Device device;
  me::Queue compute_queue;
  me::Queue graphics_queue;
  me::Queue present_queue;
  me::Queue transfer_queue;
  me::Swapchain swapchain;
  me::vector<me::SwapchainImage> swapchain_images;
  me::vector<me::Frame> frames;
  me::RenderPass render_pass;
  me::Pipeline pipeline;
  me::vector<me::Framebuffer> framebuffers;
  me::DescriptorPool descriptor_pool;
  me::vector<me::Buffer> uniform_buffers;
  me::vector<me::Descriptor> descriptors;
  me::CommandPool graphics_command_pool;
  me::CommandPool transfer_command_pool;
  me::vector<me::CommandBuffer> draw_command_buffers;

public:

  SceneRenderer();

  int initialize(const me::ModuleInfo) override;
  int terminate(const me::ModuleInfo) override;
  int tick(const me::ModuleInfo) override;

};

#endif
