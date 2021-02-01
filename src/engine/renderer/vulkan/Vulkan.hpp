#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "Types.hpp"

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../Logger.hpp"

#include <lme/vector.hpp>
#include <lme/array_proxy.hpp>

#include <vulkan/vulkan.h>

namespace me {

  class Vulkan : public RendererModule {

  protected:

    vector<const char*, uint32_t> required_extensions;
    vector<const char*, uint32_t> required_layers;

    vector<const char*, uint32_t> required_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    vector<const char*, uint32_t> required_device_layers;

    Logger logger;

    allocator alloc;

#ifndef NDEBUG
    VkDebugUtilsMessengerEXT vk_debug_utils_messenger;
    VkDebugReportCallbackEXT vk_debug_report_callback;
#endif

    bool debugging;
    VkAllocationCallbacks* vk_allocation = nullptr;
    VkInstance vk_instance;

  public:

    explicit Vulkan();

    int init_engine(
	const EngineInitInfo &engine_init_info
	) override;

    int terminate_engine(
	) override;

    int enumerate_physical_devices(
	uint32_t 						&physical_device_count,
	PhysicalDevice* 					physical_devices
	) override;

    int create_surface(
	const SurfaceCreateInfo 				&surface_create_info,
	Surface 						&surface
	) override;

    int create_device(
	const DeviceCreateInfo 					&device_create_info,
	Device 							&device
	) override;

    int create_queue(
	const QueueCreateInfo 					&queue_create_info,
	Queue 							&queue
	) override;

    int create_swapchain(
	const SwapchainCreateInfo 				&swapchain_create_info,
	Swapchain 						&swapchain
	) override;

    int create_swapchain_images(
	const SwapchainImageCreateInfo 				&swapchain_image_create_info,
	uint32_t 						swapchain_image_count,
	SwapchainImage* 					swapchain_images
	) override;

    int create_frames(
	const FrameCreateInfo 					&frame_create_info,
	uint32_t 						frame_count,
	Frame* 							frames
	) override;

    int create_buffer(
	const BufferCreateInfo 					&buffer_create_info,
	Buffer 							&buffer
	) override;

    int create_render_pass(
	const RenderPassCreateInfo 				&render_pass_create_info,
	RenderPass 						&render_pass
	) override;

    int create_pipeline(
	const PipelineCreateInfo 				&pipeline_create_info,
	Pipeline 						&pipeline
	) override;

    int create_framebuffer(
	const FramebufferCreateInfo 				&framebuffer_create_info,
	Framebuffer 						&framebuffer
	) override;

    int create_descriptor_pool(
	const DescriptorPoolCreateInfo 				&descriptor_pool_create_info,
	DescriptorPool 						&descriptor_pool
	) override;

    int create_descriptors(
	const DescriptorCreateInfo 				&descriptor_create_info,
	uint32_t 						descriptor_count,
	Descriptor* 						descriptors
	) override;

    int create_command_pool(
	const CommandPoolCreateInfo 				&command_create_pool_info,
	CommandPool 						&command_pool
	) override;

    int create_command_buffers(
	const CommandBufferCreateInfo 				&command_buffer_create_info,
	uint32_t 						command_buffer_count,
	CommandBuffer* 						command_buffers
	) override;


    int buffer_write(
	const BufferWriteInfo 					&buffer_write_info,
	Buffer 							buffer
	) override;

    
    int cmd_record_start(
	const CommandInfo 					&command_info,
	CommandBuffer 						command_buffer
	) override;

    int cmd_record_stop(
	const CommandInfo 					&command_info,
	CommandBuffer 						command_buffer
	) override;

    int cmd_begin_render_pass(
	const CmdBeginRenderPassInfo 				&cmd_begin_render_pass_info,
	CommandBuffer 						command_buffer
	) override;

    int cmd_end_render_pass(
	CommandBuffer 						command_buffer
	) override;

    int cmd_bind_descriptors(
	const CmdBindDescriptorsInfo 				&cmd_bind_descriptors_info,
	CommandBuffer 						command_buffer
	) override;

    int cmd_draw_meshes(
	const CmdDrawMeshesInfo					&cmd_draw_meshes_info,
	CommandBuffer 						command_buffer
	) override;

    int frame_prepare(
	const FramePrepareInfo 					&frame_prepare_info,
	FramePresented 						&frame_prepared
	) override;

    int frame_render(
	const FrameRenderInfo					&frame_render_info,
	FrameRendered 						&frame_rendered
	) override;

    int frame_present(
	const FramePresentInfo 					&frame_present_info,
	FramePresented 						&frame_presented
	) override;

    int get_swapchain_image_count(
	Device 							device,
	Swapchain 						swapchain,
	uint32_t 						&image_count
	) override;

    int frame_prepared_get_image_index(
	FramePrepared 						frame_prepared,
	uint32_t 						&image_index
	) override;

    int setup_mesh(
	const SetupMeshInfo 					&setup_mesh_info,
	Mesh* 							mesh
	) override;

  protected:

    int initialize(const ModuleInfo module_info) override;
    int terminate(const ModuleInfo module_info) override;
    int tick(const ModuleInfo module_info) override;

    int setup_instance(const EngineInitInfo &engine_init_info);
    int cleanup_instance();

  public:

    int cleanup_surface(
	const SurfaceCleanupInfo &surface_cleanup_info,
	Surface surface
	) override;

    int cleanup_device(
	const DeviceCleanupInfo &device_cleanup_info,
	Device device
	) override;

    int cleanup_swapchain(
	const SwapchainCleanupInfo &swapchain_cleanup_info,
	Swapchain swapchain
	) override;

    int cleanup_swapchain_images(
	const SwapchainImageCleanupInfo &swapchain_image_cleanup_info,
	uint32_t swapchain_image_count,
	SwapchainImage* swapchain_images
	) override;

    int cleanup_frames(
	const FrameCleanupInfo &frame_cleanup_info,
	uint32_t frame_count,
	Frame* frames
	) override;

    int cleanup_render_pass(
	const RenderPassCleanupInfo &render_pass_cleanup_info,
	RenderPass render_pass
	) override;

    int cleanup_pipeline(
	const PipelineCleanupInfo &pipeline_cleanup_info,
	Pipeline pipeline
	) override;

    int cleanup_framebuffer(
	const FramebufferCleanupInfo &framebuffer_cleanup_info,
	Framebuffer framebuffer
	) override;

    int cleanup_descriptor_pool(
	const DescriptorPoolCleanupInfo &descriptor_pool_cleanup_info,
	DescriptorPool descriptor_pool
	) override;

    int cleanup_descriptors(
	const DescriptorCleanupInfo &descriptor_cleanup_info,
	uint32_t descriptor_count,
	Descriptor* descriptors
	) override;

    int cleanup_command_pool(
	const CommandPoolCleanupInfo &command_pool_cleanup_info,
	CommandPool command_pool
	) override;

    int cleanup_command_buffers(
	const CommandBufferCleanupInfo &command_buffer_cleanup_info,
	uint32_t command_buffer_count,
	CommandBuffer* command_buffers
	) override;

  protected:

#ifndef NDEBUG
    int setup_debug();
    int cleanup_debug();
#endif

    /* callback functions */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
      VkDebugReportFlagsEXT 					debug_report_flags,
      VkDebugReportObjectTypeEXT 				debug_report_object_type,
      uint64_t 							object,
      size_t 							location,
      int32_t 							message_code,
      const char* 						layer_prefix,
      const char* 						message,
      void* 							user_data
      );

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT 			message_severity,
    	VkDebugUtilsMessageTypeFlagsEXT 			message_type,
    	const VkDebugUtilsMessengerCallbackDataEXT* 		callback_data, 
    	void* 							user_data
    	);

    static int surface_resize_callback(
	uint32_t 						width,
	uint32_t 						height,
	void* 							ptr
	);

  };

}

#endif
