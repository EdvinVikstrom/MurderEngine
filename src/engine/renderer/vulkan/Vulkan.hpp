#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "VulkanAlloc.hpp"

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../Logger.hpp"

#include <vulkan/vulkan.h>

#include <vulkan/vulkan_core.h>

#include <lme/optional.hpp>
#include <lme/array.hpp>

namespace me {

  class Vulkan : public Renderer {

  /* structs */
  protected:

    struct VertexInfo;
    struct MeshInfo;

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

    struct MemoryInfo;
    struct DataStorage;

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

    MemoryInfo memory_info;
    DataStorage data_storage;
    RenderInfo render_info;
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

  public:

    explicit Vulkan(Surface* me_surface);

    int register_shader(Shader* shader) override;
    int register_mesh(Mesh* mesh) override;

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
    int setup_debug_messenger();
    int setup_debug_report();
    int setup_surface();
    int setup_physical_device();
    int setup_logical_device();
    int setup_memory();
    int setup_swapchain();
    int setup_image_views();
    int setup_render_pass();
    int setup_shaders();
    int setup_pipeline_layout();
    int setup_graphics_pipeline();
    int setup_framebuffers();
    int setup_command_pool();
    int setup_vertex_buffers();
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
    int cleanup_debug_messenger();
    int cleanup_debug_report();
    int cleanup_logical_device();
    int cleanup_memory();
    int cleanup_surface();
    int cleanup_swapchain();
    int cleanup_image_views();
    int cleanup_pipeline_layout();
    int cleanup_render_pass();
    int cleanup_shaders();
    int cleanup_graphics_pipeline();
    int cleanup_framebuffers();
    int cleanup_command_pool();
    int cleanup_vertex_buffers();
    int cleanup_command_buffers();
    int cleanup_synchronization();

    int create_shader_module(const Shader*);
    int create_vertex_buffer(Mesh*);

    int get_physical_device_infos(const array_proxy<VkPhysicalDevice>&, PhysicalDeviceInfo*);

    static bool has_extensions(const array_proxy<VkExtensionProperties>&, const vector<const char*> &required_extensions);
    static bool has_layers(const array_proxy<VkLayerProperties>&, const vector<const char*> &required_layers);
    static bool has_queue_families(const array_proxy<VkQueueFamilyProperties>&, const vector<VkQueueFlags> &required_queue_family_properties);
    static bool has_present_mode(const array_proxy<VkPresentModeKHR>&, const VkPresentModeKHR required_present_mode);

    static int find_surface_format(const VkFormat, const VkColorSpaceKHR, const array_proxy<VkSurfaceFormatKHR>&, VkSurfaceFormatKHR&);
    static int find_present_mode(const VkPresentModeKHR, const array_proxy<VkPresentModeKHR>&, VkPresentModeKHR&);
    static int find_image_extent(const VkExtent2D &current_extent, VkExtent2D&);
    static int find_queue_families(const VkPhysicalDevice, const VkSurfaceKHR, const array_proxy<VkQueueFamilyProperties>&, QueueFamilyIndices&);

    static int get_extent(const VkExtent2D max_extent, const VkExtent2D min_extent, VkExtent2D &extent);
    static int get_memory_type(const VkPhysicalDevice, uint32_t type_filter, VkMemoryPropertyFlags, uint32_t &memory_type);

    /* callbacks */
    static int callback_surface_resize(uint32_t width, uint32_t height, void* ptr);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT,
	uint64_t object, size_t location, int32_t message_code,
	const char* layer_prefix, const char* message, void* user_data);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
	const VkDebugUtilsMessengerCallbackDataEXT*, void* user_data);

  protected:

#ifndef NDEBUG

#endif

  };

}

#endif
