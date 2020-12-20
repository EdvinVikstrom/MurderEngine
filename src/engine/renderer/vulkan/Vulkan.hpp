#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "VulkanMemory.hpp"

#include "../Renderer.hpp"
#include "../../surface/VulkanSurface.hpp"
#include "../../memory/MemoryPool.hpp"

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
    struct CommandPoolInfo;
    struct SurfaceInfo;
    struct SwapchainInfo;
    struct RenderPassInfo;
    struct PipelineLayoutInfo;
    struct GraphicsPipelineInfo;
    struct ViewportInfo;
    struct RasterizerInfo;
    struct MultisamplingInfo;
    struct ColorBlendInfo;
    struct DynamicInfo;
    struct ShaderInfo;
    struct Storage;
    struct Temp;

#include "Structs.hpp"

  protected:

    Logger* logger;

    VulkanAlloc alloc;

    mutable Storage storage;
    mutable Temp temp;

    const VulkanSurface &me_surface;
    vector<RenderLayer*> layers;
    vector<const char*> required_device_extensions;

    InstanceInfo instance_info;
    PhysicalDeviceInfo physical_device_info;
    LogicalDeviceInfo logical_device_info;
    CommandPoolInfo command_pool_info;
    SurfaceInfo surface_info;
    SwapchainInfo swapchain_info;
    RenderPassInfo render_pass_info;
    PipelineLayoutInfo pipeline_layout_info;
    GraphicsPipelineInfo graphics_pipeline_info;

    ViewportInfo viewport_info;
    RasterizerInfo rasterizer_info;
    MultisamplingInfo multisampling_info;
    ColorBlendInfo color_blend_info;
    DynamicInfo dynamic_info;
    ShaderInfo shader_info;

  public:

    explicit Vulkan(const MurderEngine* engine, const VulkanSurface &me_surface);

    int queue_shader(Shader* shader) const override;
    int compile_shader(Shader* shader) const override;
    int register_shader(Shader* shader) const override;

  protected:

    int initialize() override;
    int terminate() override;

    int setup_instance();
    int setup_physical_device();
    int setup_logical_device();
    int setup_command_pool();
    int setup_surface();
    int setup_swapchain();
    int setup_pipeline_layout();
    int setup_render_pass();
    int setup_shaders();
    int setup_graphics_pipeline();

    int setup_viewports();
    int setup_rasterizer();
    int setup_multisampling();
    int setup_color_blend();
    int setup_dynamic();

    int get_physical_device_infos(uint32_t &physical_device_count, VkPhysicalDevice*, PhysicalDeviceInfo*);

    /* physical device */
    static int get_physical_device_queue_family(const VkPhysicalDevice, QueueFamilyIndices&);
    static int get_physical_device_features(const VkPhysicalDevice, VkPhysicalDeviceFeatures&);
    static int get_physical_device_extensions(const VkPhysicalDevice, uint32_t &extension_count, VkExtensionProperties*);
    static int get_physical_device_properties(const VkPhysicalDevice, VkPhysicalDeviceProperties&);
    static int get_physical_devices(const VkInstance, uint32_t &physical_device_count, VkPhysicalDevice*);

    /* surface */
    static int get_surface_formats(const VkPhysicalDevice, const VkSurfaceKHR, uint32_t &count, VkSurfaceFormatKHR*);
    static int get_surface_present_modes(const VkPhysicalDevice, const VkSurfaceKHR, uint32_t &count, VkPresentModeKHR*);
    static int get_surface_capabilities(const VkPhysicalDevice, const VkSurfaceKHR, VkSurfaceCapabilitiesKHR&);


    static bool has_required_extensions(const PhysicalDeviceInfo&, const vector<const char*> &required_extensions);
    static bool has_present_mode(const uint32_t present_mode_count, const VkPresentModeKHR* present_modes, const VkPresentModeKHR present_mode);

    static int find_surface_format(const VkFormat color_format, const VkColorSpaceKHR color_space, const uint32_t format_count, const VkSurfaceFormatKHR*, VkSurfaceFormatKHR&);

    static int get_extension_names(uint32_t extension_count, const VkExtensionProperties* extensions, const char** extension_names);
    static int get_surface_extent(const VkExtent2D max_extent, const VkExtent2D min_extent, VkExtent2D &extent);
    static int get_shader_stage_flag(const ShaderType, VkShaderStageFlagBits&);

    static int get_logical_device_queue_create_info(const uint32_t family_index, const uint32_t queue_count, const float* queue_priorities, VkDeviceQueueCreateInfo&);

  };

}

#endif
