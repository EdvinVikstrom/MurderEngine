#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

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
    struct InstanceCreateInfo;
    struct PhysicalDeviceInfo;
    struct PhysicalDeviceCreateInfo;
    struct LogicalDeviceInfo;
    struct LogicalDeviceCreateInfo;
    struct CommandPoolInfo;
    struct CommandPoolCreateInfo;
    struct SurfaceInfo;
    struct SurfaceCreateInfo;
    struct SwapchainInfo;
    struct SwapchainCreateInfo;
    struct PipelineLayoutInfo;
    struct PipelineLayoutCreateInfo;
    struct RenderPassInfo;
    struct RenderPassCreateInfo;
    struct GraphicsPipelineInfo;
    struct GraphicsPipelineCreateInfo;
    struct ViewportInfo;
    struct RasterizerInfo;
    struct MultisamplingInfo;
    struct ColorBlendInfo;
    struct DynamicInfo;
    struct Storage;
    struct Temp;

#include "Structs.hpp"

  protected:

    Logger* logger;

    vector<const char*> extensions;

    mutable Storage storage;
    mutable Temp temp;

    const VulkanSurface &me_surface;

    InstanceInfo instance_info;
    PhysicalDeviceInfo physical_device_info;
    LogicalDeviceInfo logical_device_info;
    CommandPoolInfo command_pool_info;
    SurfaceInfo surface_info;
    SwapchainInfo swapchain_info;
    PipelineLayoutInfo pipeline_layout_info;
    RenderPassInfo render_pass_info;

    ViewportInfo viewport_info;
    RasterizerInfo rasterizer_info;
    MultisamplingInfo multisampling_info;
    ColorBlendInfo color_blend_info;
    DynamicInfo dynamic_info;

  public:

    explicit Vulkan(const MurderEngine* engine, const VulkanSurface &me_surface);

    int compile_shader(Shader* shader) const override;

  protected:

    int initialize() override;
    int terminate() override;
    int tick(const Context context) override;

    int setup_instance();
    int setup_physical_device();
    int setup_surface();
    int setup_logical_device();
    int setup_command_pool();
    int setup_swapchain();
    int setup_pipeline_layout();
    int setup_render_pass();

    int setup_viewports();
    int setup_rasterizer();
    int setup_multisampling();
    int setup_color_blend();
    int setup_dynamic();

    int create_instance(InstanceCreateInfo &instance_create_info, InstanceInfo &instance_info);
    int create_physical_device(PhysicalDeviceCreateInfo &physical_device_create_info, PhysicalDeviceInfo &physical_device_info);
    int create_surface(SurfaceCreateInfo &surface_create_info, SurfaceInfo &surface_info);
    int create_logical_device(LogicalDeviceCreateInfo &logical_device_create_info, LogicalDeviceInfo &logical_device_info);
    int create_command_pool(CommandPoolCreateInfo &command_pool_create_info, CommandPoolInfo &command_pool_info);
    int create_swapchain(SwapchainCreateInfo &swapchain_create_info, SwapchainInfo &swapchain_info);
    int create_pipeline_layout(PipelineLayoutCreateInfo &pipeline_layout_create_info, PipelineLayoutInfo &pipeline_layout_info);
    int create_render_pass(RenderPassCreateInfo &render_pass_create_info, RenderPassInfo &render_pass_info);

    int create_viewports(ViewportInfo &viewport_info);
    int create_rasterizer(RasterizerInfo &rasterizer_info);
    int create_multisampling(MultisamplingInfo &multisampling_info);
    int create_color_blend(ColorBlendInfo &color_blend_info);
    int create_dynamic(DynamicInfo &dynamic_info);

    /* physical device */
    int get_physical_device_queue_family(const VkPhysicalDevice physical_device, QueueFamilyIndices &indices);
    int get_physical_device_features(const VkPhysicalDevice physical_device, VkPhysicalDeviceFeatures &features);
    int get_physical_device_extensions(const VkPhysicalDevice physical_device, vector<VkExtensionProperties> &extensions);
    int get_physical_device_properties(const VkPhysicalDevice physical_device, VkPhysicalDeviceProperties &properties);
    int get_physical_devices(vector<VkPhysicalDevice> &physical_devices);

    /* surface */
    int get_surface_formats(const VkSurfaceKHR surface, vector<VkSurfaceFormatKHR> &formats);
    int get_surface_present_modes(const VkSurfaceKHR surface, vector<VkPresentModeKHR> &modes);
    int get_surface_capabilities(const VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR &capabilities);



  };

}

#endif
