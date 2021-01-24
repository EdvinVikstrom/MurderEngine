#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../Logger.hpp"

#include <lme/vector.hpp>
#include <lme/array_proxy.hpp>

#include <vulkan/vulkan.h>

namespace me::vulkan {

  class Vulkan : public Renderer {

  protected:

    struct QueueFamilyIndices {
      uint32_t compute_queue = -1;
      uint32_t graphics_queue = -1;
      uint32_t present_queue = -1;
      uint32_t transfer_queue = -1;
    };

    struct QueueFamilies {
      VkQueue compute_queue;
      VkQueue graphics_queue;
      VkQueue present_queue;
      VkQueue transfer_queue;
    };

    struct SwapchainImage {
      VkImage image;
      VkImageView image_view;
      vector<VkFramebuffer, uint32_t> vk_framebuffers;
      vector<VkBuffer, uint32_t> vk_uniform_buffers;
      vector<VkDescriptorSet, uint32_t> vk_descriptor_sets; /* supports max 1. see setup_descriptor_pool */
      vector<VkCommandBuffer, uint32_t> vk_command_buffers;

      vector<VkDeviceMemory, uint32_t> vk_uniform_buffers_memory;
    };

    struct DataStorage {
      vector<MeshRef*, uint32_t> meshes;
    };

    struct RenderInfo {
      enum Flag : uint8_t {
        FRAMEBUFFER_RESIZED_FLAG_BIT = 1
      };
    
      enum State : uint8_t {
        IDLE_STATE,
        ACTIVE_STATE,
        NO_SWAPCHAIN_STATE
      };
    
      uint8_t flags = 0;
      State state = IDLE_STATE;
      uint32_t frame_index = 0;
      uint32_t image_index = 0;
      static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    };

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

    VkAllocationCallbacks* vk_allocation = nullptr;
    VkInstance vk_instance;
    VkSurfaceKHR vk_surface;
    VkSurfaceCapabilitiesKHR vk_surface_capabilities;
    VkSurfaceFormatKHR vk_surface_format;
    VkPresentModeKHR vk_surface_present_mode;
    VkExtent2D vk_surface_extent;
    VkPhysicalDevice vk_physical_device;
    VkDevice vk_device;
    VkSwapchainKHR vk_swapchain;
    VkFormat vk_swapchain_image_format;
    VkColorSpaceKHR vk_swapchain_image_color_space;
    VkExtent2D vk_swapchain_image_extent;
    VkRenderPass vk_render_pass;
    VkPipeline vk_pipeline;
    VkPipelineLayout vk_pipeline_layout;
    VkDescriptorSetLayout vk_descriptor_set_layout;
    VkDeviceMemory vk_staging_buffer_memory;
    VkDeviceMemory vk_vertex_buffer_memory;
    VkDeviceMemory vk_index_buffer_memory;
    vector<VkDeviceMemory, uint32_t> vk_uniform_buffers_memory;
    VkCommandPool vk_graphics_command_pool;
    VkCommandPool vk_transfer_command_pool;
    VkDescriptorPool vk_descriptor_pool;
    vector<SwapchainImage, uint32_t> swapchain_images;

    QueueFamilyIndices queue_family_indices;
    QueueFamilies queue_families;

    vector<VkSemaphore, uint32_t> vk_image_available_semaphores;
    vector<VkSemaphore, uint32_t> vk_render_finished_semaphores;
    vector<VkFence, uint32_t> vk_in_flight_fences;
    vector<VkFence, uint32_t> vk_images_in_flight_fences;

    vector<SurfaceInfo, uint32_t> surface_infos;
    vector<SwapchainInfo, uint32_t> swapchain_infos;
    vector<FramebufferInfo, uint32_t> framebuffer_infos;
    vector<UniformBufferInfo, uint32_t> uniform_buffer_infos;
    vector<DescriptorInfo, uint32_t> descriptor_infos;
    vector<CommandBufferInfo, uint32_t> command_buffer_infos;

    DataStorage data_storage;
    RenderInfo render_info;

  public:

    explicit Vulkan();

    int init_engine(const EngineInfo &engine_info, Surface* surface) override;
    int setup_device(const DeviceInfo &device_info, Device &_device) override;
    int setup_surface(const SurfaceInfo &surface_info) override;
    int setup_swapchain(const SwapchainInfo &swapchain_info, Swapchain &_swapchain) override;
    int setup_memory(const MemoryInfo &memory_info, Memory &_memory) override;
    int setup_render_pass(const RenderPassInfo &render_pass_info, RenderPass &_render_pass) override;
    int setup_pipeline(const PipelineInfo &pipeline_info, Pipeline &_pipeline) override; /* can only setup 1 pipeline */
    int setup_framebuffer(const FramebufferInfo &framebuffer_info, Framebuffer &_framebuffer) override;
    int setup_uniform_buffer(const UniformBufferInfo &uniform_buffer_info, UniformBuffer &_uniform_buffer) override;
    int setup_descriptor(const DescriptorInfo &descriptor_info, Descriptor &_descriptor) override;
    int setup_command_buffer(const CommandBufferInfo &command_buffer_info, CommandBuffer &_command_buffer) override;

    int register_mesh(MeshRef* mesh) override;

  protected:

    int initialize(const ModuleInfo module_info) override;
    int terminate(const ModuleInfo module_info) override;
    int tick(const ModuleInfo module_info) override;

    int render(RenderInfo &render_info);

    int setup_instance(const EngineInfo &engine_info, Surface* surface);

    int cleanup_instance();
    int cleanup_surface();
    int cleanup_device();
    int cleanup_swapchain();
    int cleanup_memory();
    int cleanup_render_pass();
    int cleanup_pipeline(const array_proxy<Pipeline, uint32_t> &_pipelines);
    int cleanup_framebuffers(const array_proxy<Framebuffer, uint32_t> &_framebuffers);
    int cleanup_uniform_buffers(const array_proxy<UniformBuffer, uint32_t> &_uniform_buffers);
    int cleanup_descriptor_pool();
    int cleanup_descriptor_sets(const array_proxy<Descriptor, uint32_t> &_descriptors);
    int cleanup_command_pool();
    int cleanup_command_buffers(const array_proxy<CommandBuffer, uint32_t> &_command_buffers);

    int refresh_swapchain();
    int refresh_uniform_buffers(const RenderInfo &render_info);

    int setup_mesh(const MeshInfo &mesh_info, Mesh* mesh);
    int cleanup_mesh(Mesh* mesh);

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
