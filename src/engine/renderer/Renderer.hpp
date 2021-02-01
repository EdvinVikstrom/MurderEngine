#ifndef ME_RENDERER_HPP
  #define ME_RENDERER_HPP

#include "Types.hpp"

#include "../Module.hpp"
#include "../scene/Mesh.hpp"

#ifndef VERIFY_CREATE_INFO
  #define VERIFY_CREATE_INFO(i, t) {\
    if (i.type != t) \
      throw exception("wrong structure type '%s'. expected structure type '%s'", \
	  me::structure_type_name(i.type), me::structure_type_name(t)); \
  }
#endif

namespace me {

  class RendererModule : public Module {

  protected:

  public:

    RendererModule(const string &name)
      : Module(MODULE_RENDERER_TYPE, name)
    {
    }

    virtual int init_engine(
	const EngineInitInfo 					&engine_init_info
	) = 0;

    virtual int terminate_engine(
	) = 0;

    virtual int enumerate_physical_devices(
	uint32_t 						&physical_device_count,
	PhysicalDevice* 					physical_devices
	) = 0;

    /* create functions */
    virtual int create_surface(
	const SurfaceCreateInfo 				&surface_create_info,
	Surface 						&surface
	) = 0;

    virtual int create_device(
	const DeviceCreateInfo 					&device_create_info,
	Device 							&device
	) = 0;

    virtual int create_queue(
	const QueueCreateInfo 				&queue_create_info,
	Queue 							&queue
	) = 0;

    virtual int create_swapchain(
	const SwapchainCreateInfo 				&swapchain_create_info,
	Swapchain 						&swapchain
	) = 0;

    virtual int create_swapchain_images(
	const SwapchainImageCreateInfo 				&swapchain_image_create_info,
	uint32_t 						swapchain_image_count,
	SwapchainImage* 					swapchain_images
	) = 0;

    virtual int create_frames(
	const FrameCreateInfo 					&frame_create_info,
	uint32_t 						frame_count,
	Frame* 							frames
	) = 0;

    virtual int create_buffer(
	const BufferCreateInfo 					&buffer_create_info,
	Buffer 							&buffer
	) = 0;
    
    virtual int create_render_pass(
	const RenderPassCreateInfo 				&render_pass_create_info,
	RenderPass 						&render_pass
	) = 0;

    virtual int create_pipeline(
	const PipelineCreateInfo 				&pipeline_create_info,
	Pipeline 						&pipeline
	) = 0;

    virtual int create_framebuffer(
	const FramebufferCreateInfo 				&framebuffer_create_info,
	Framebuffer 						&framebuffer
	) = 0;

    virtual int create_descriptor_pool(
	const DescriptorPoolCreateInfo 				&descriptor_pool_create_info,
	DescriptorPool 						&descriptor_pool
	) = 0;

    virtual int create_descriptors(
	const DescriptorCreateInfo 				&descriptor_create_info,
	uint32_t 						descriptor_count,
	Descriptor* 						descriptors
	) = 0;

    virtual int create_command_pool(
	const CommandPoolCreateInfo 				&command_pool_create_info,
	CommandPool 						&command_pool
	) = 0;

    virtual int create_command_buffers(
	const CommandBufferCreateInfo 				&command_buffer_create_info,
	uint32_t 						command_buffer_count,
	CommandBuffer* 						command_buffers
	) = 0;

    /* cleanup functions */
    virtual int cleanup_surface(
	const SurfaceCleanupInfo &surface_cleanup_info,
	Surface surface
	) = 0;

    virtual int cleanup_device(
	const DeviceCleanupInfo &device_cleanup_info,
	Device device
	) = 0;

    virtual int cleanup_swapchain(
	const SwapchainCleanupInfo &swapchain_cleanup_info,
	Swapchain swapchain
	) = 0;

    virtual int cleanup_swapchain_images(
	const SwapchainImageCleanupInfo &swapchain_image_cleanup_info,
	uint32_t swapchain_image_count,
	SwapchainImage* swapchain_images
	) = 0;

    virtual int cleanup_frames(
	const FrameCleanupInfo &frame_cleanup_info,
	uint32_t frame_count,
	Frame* frames
	) = 0;

    virtual int cleanup_render_pass(
	const RenderPassCleanupInfo &render_pass_cleanup_info,
	RenderPass render_pass
	) = 0;

    virtual int cleanup_pipeline(
	const PipelineCleanupInfo &pipeline_cleanup_info,
	Pipeline pipeline
	) = 0;

    virtual int cleanup_framebuffer(
	const FramebufferCleanupInfo &framebuffer_cleanup_info,
	Framebuffer framebuffer
	) = 0;

    virtual int cleanup_descriptor_pool(
	const DescriptorPoolCleanupInfo &descriptor_pool_cleanup_info,
	DescriptorPool descriptor_pool
	) = 0;

    virtual int cleanup_descriptors(
	const DescriptorCleanupInfo &descriptor_cleanup_info,
	uint32_t descriptor_count,
	Descriptor* descriptors
	) = 0;

    virtual int cleanup_command_pool(
	const CommandPoolCleanupInfo &command_pool_cleanup_info,
	CommandPool command_pool
	) = 0;

    virtual int cleanup_command_buffers(
	const CommandBufferCleanupInfo &command_buffer_cleanup_info,
	uint32_t command_buffer_count,
	CommandBuffer* command_buffers
	) = 0;

    /* buffer functions */
    virtual int buffer_write(
	const BufferWriteInfo 					&buffer_write_info,
	Buffer 							buffer
	) = 0;

    /* command functions */
    virtual int cmd_record_start(
	const CommandInfo 					&command_info,
	CommandBuffer 						command_buffer
	) = 0;

    virtual int cmd_record_stop(
	const CommandInfo 					&command_info,
	CommandBuffer 						command_buffer
	) = 0;

    virtual int cmd_begin_render_pass(
	const CmdBeginRenderPassInfo 				&cmd_begin_render_pass_info,
	CommandBuffer 						command_buffer
	) = 0;

    virtual int cmd_end_render_pass(
	CommandBuffer 						command_buffer
	) = 0;

    virtual int cmd_bind_descriptors(
	const CmdBindDescriptorsInfo 				&cmd_bind_descriptors_info,
	CommandBuffer 						command_buffer
	) = 0;

    virtual int cmd_draw_meshes(
	const CmdDrawMeshesInfo					&cmd_draw_meshes_info,
	CommandBuffer 						command_buffer
	) = 0;

    /* frame functions */
    virtual int frame_prepare(
	const FramePrepareInfo 					&frame_prepare_info,
	FramePresented 						&frame_prepared
	) = 0;

    virtual int frame_render(
	const FrameRenderInfo					&frame_render_info,
	FrameRendered 						&frame_rendered
	) = 0;

    virtual int frame_present(
	const FramePresentInfo 					&frame_present_info,
	FramePresented 						&frame_presented
	) = 0;

    /* getters */
    virtual int get_swapchain_image_count(
	Device 							device,
	Swapchain 						swapchain,
	uint32_t 						&image_count
	) = 0;

    virtual int frame_prepared_get_image_index(
	FramePrepared 						frame_prepared,
	uint32_t 						&image_index
	) = 0;

    /* setup */
    virtual int setup_mesh(
	const SetupMeshInfo 					&setup_mesh_info,
	Mesh* 							mesh
	) = 0;

  };

}

#endif
