#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "VulkanMemory.hpp"

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../Logger.hpp"

#include <vulkan/vulkan.h>

#include <vulkan/vulkan_core.h>

#include <lme/optional.hpp>
#include <lme/static_vector.hpp>

namespace me {

  class Vulkan : public Renderer {

  /* structs */
  protected:

    struct QueueFamilyIndices;
    struct InstanceInfo;
    struct PhysicalDeviceInfo;
    struct LogicalDeviceInfo;
    struct QueueInfo;
    struct SurfaceInfo;
    struct SwapchainInfo;
    struct RenderPassInfo;
    struct PipelineLayoutInfo;
    struct GraphicsPipelineInfo;
    struct FramebufferInfo;
    struct CommandPoolInfo;
    struct SynchronizationInfo;
    struct ViewportInfo;
    struct RasterizerInfo;
    struct MultisamplingInfo;
    struct ColorBlendInfo;
    struct DynamicInfo;
    struct ShaderInfo;
    struct ShadersInfo;
    struct RenderInfo;

#ifndef NDEBUG
    struct DebugInfo;
#endif

#include "Structs.hpp"

  protected:

    Logger logger;

    VulkanAlloc alloc;

    Surface* me_surface;
    vector<const char*> required_extensions;
    vector<const char*> required_layers;

    vector<const char*> required_device_extensions;
    vector<const char*> required_device_layers;
    vector<VkQueueFlags> required_queue_family_properties;

    InstanceInfo instance_info;
    PhysicalDeviceInfo physical_device_info;
    LogicalDeviceInfo logical_device_info;
    QueueInfo queue_info;
    SurfaceInfo surface_info;
    SwapchainInfo swapchain_info;
    RenderPassInfo render_pass_info;
    PipelineLayoutInfo pipeline_layout_info;
    GraphicsPipelineInfo graphics_pipeline_info;
    FramebufferInfo framebuffer_info;
    CommandPoolInfo command_pool_info;
    VkArray<VkCommandBuffer> command_buffers;
    SynchronizationInfo synchronization_info;

    ViewportInfo viewport_info;
    RasterizerInfo rasterizer_info;
    MultisamplingInfo multisampling_info;
    ColorBlendInfo color_blend_info;
    DynamicInfo dynamic_info;
    ShaderInfo shader_info;

#ifndef NDEBUG
    DebugInfo debug_info;
#endif

    RenderInfo render_info;
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

  public:

    explicit Vulkan(Surface* me_surface);

    int register_shader(Shader* shader) override;

  protected:

    int initialize(const ModuleInfo module_info) override;
    int terminate(const ModuleInfo module_info) override;
    int tick(const ModuleInfo module_info) override;

    int render(RenderInfo &render_info);

    /* setup functions */
    int setup_extensions();
    int setup_layers();
    int setup_device_extensions();
    int setup_device_layers();
    int setup_instance(const EngineInfo* engine_info);
    int setup_me_surface();
    int setup_physical_device();
    int setup_logical_device();
    int setup_surface();
    int setup_swapchain();
    int setup_image_views();
    int setup_pipeline_layout();
    int setup_render_pass();
    int setup_shaders();
    int setup_graphics_pipeline();
    int setup_framebuffers();
    int setup_command_pool();
    int setup_command_buffers();
    int setup_synchronization();

    int refresh_swapchains();

    int start_render_pass(VkCommandBuffer command_buffer, VkFramebuffer framebuffer);

    int setup_viewports();
    int setup_rasterizer();
    int setup_multisampling();
    int setup_color_blend();
    int setup_dynamic();

    /* cleanup functions */
    int cleanup_instance();
    int cleanup_logical_device();
    int cleanup_surface();
    int cleanup_swapchain();
    int cleanup_image_views();
    int cleanup_pipeline_layout();
    int cleanup_render_pass();
    int cleanup_shaders();
    int cleanup_graphics_pipeline();
    int cleanup_framebuffers();
    int cleanup_command_pool();
    int cleanup_command_buffers();
    int cleanup_synchronization();

    int get_physical_device_infos(uint32_t &physical_device_count, VkPhysicalDevice*, PhysicalDeviceInfo*);

    int create_shader_module(const Shader*);

    static bool has_extensions(const VkArray<VkExtensionProperties>&, const vector<const char*> &required_extensions);
    static bool has_layers(const VkArray<VkLayerProperties>&, const vector<const char*> &required_layers);
    static bool has_queue_families(const VkArray<VkQueueFamilyProperties>&, const vector<VkQueueFlags> &required_queue_family_properties);
    static bool has_present_mode(const VkArray<VkPresentModeKHR>&, const VkPresentModeKHR required_present_mode);

    static int find_surface_format(const VkFormat color_format, const VkColorSpaceKHR color_space, const VkArray<VkSurfaceFormatKHR>&, VkSurfaceFormatKHR&);
    static int find_present_mode(const VkPresentModeKHR, const VkArray<VkPresentModeKHR>&, VkPresentModeKHR&);
    static int find_image_extent(const VkExtent2D &current_extent, VkExtent2D&);
    static int find_queue_families(const VkPhysicalDevice, const VkSurfaceKHR, const uint32_t queue_family_property_count, const VkQueueFamilyProperties*, QueueFamilyIndices&);

    static int get_extent(const VkExtent2D max_extent, const VkExtent2D min_extent, VkExtent2D &extent);
    static int get_shader_stage_flag(const ShaderType, VkShaderStageFlagBits&);

    static int get_logical_device_queue_create_info(const uint32_t family_index, const uint32_t queue_count, const float* queue_priorities, VkDeviceQueueCreateInfo&);

    /* callbacks */
    static int callback_surface_resize(uint32_t width, uint32_t height, void* ptr);

  protected:

#ifndef NDEBUG
    int setup_debug_messenger();
    int setup_debug_report();

    int terminate_debug_messenger();
    int terminate_debug_report();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT,
	uint64_t object, size_t location, int32_t message_code,
	const char* layer_prefix, const char* message, void* user_data);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
	const VkDebugUtilsMessengerCallbackDataEXT*, void* user_data);
#endif

  };

}

#endif
