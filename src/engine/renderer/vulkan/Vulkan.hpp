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

    struct QueueFamilyIndices {
      optional<uint32_t> graphics;
    };
    
    struct PhysicalDeviceInfo {
      VkPhysicalDevice device;
      VkPhysicalDeviceProperties properties;
      VkPhysicalDeviceFeatures features;
      vector<VkExtensionProperties> extensions;
      QueueFamilyIndices queue_family_indices;
    };

    struct SurfaceInfo {
      VkSurfaceCapabilitiesKHR capabilities;
      vector<VkSurfaceFormatKHR> formats;
      vector<VkPresentModeKHR> present_modes;

      VkSurfaceFormatKHR format;
      VkPresentModeKHR present_mode;

      VkExtent2D extent;
    };

    struct SwapchainInfo {
      VkSwapchainKHR swapchain;
      VkExtent2D extent;
      vector<VkImage> images;
      vector<VkImageView> image_views;
    };

    struct PipelineLayoutInfo {
      VkPipelineLayout pipeline_layout;
      VkPipelineLayoutCreateInfo pipeline_layout_create_info;
    };

    struct ViewportInfo {
      vector<VkViewport> viewports;
      vector<VkRect2D> scissors;
      VkPipelineViewportStateCreateInfo pipline_viewport_stage_create_info;
    };

    struct RasterizerInfo {
      VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info;
    };

    struct MultisamplingInfo {
      VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info;
    };

    struct ColorBlendInfo {
      vector<VkPipelineColorBlendAttachmentState> pipeline_color_blend_attachment_states;
      VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info;
    };

    struct DynamicInfo {
      vector<VkDynamicState> dynamics;
      VkPipelineDynamicStateCreateInfo pipline_dynamic_state_create_info;
    };

    struct Storage {
      vector<VkShaderModule> shaders;
    };

    struct Temp {
      vector<VkPipelineShaderStageCreateInfo> pipeline_shader_stage_create_infos;
    };

  protected:

    Logger* logger;

    const VulkanSurface &surface_instance;
    vector<const char*> extensions;

    mutable Storage storage;
    mutable Temp temp;

    VkInstance instance;
    VkSurfaceKHR surface;
    PhysicalDeviceInfo physical_device_info = { VK_NULL_HANDLE };
    VkDevice device;
    VkCommandPool command_pool;
    SurfaceInfo surface_info;
    SwapchainInfo swapchain_info;
    PipelineLayoutInfo pipeline_layout_info;
    ViewportInfo viewport_info;
    RasterizerInfo rasterizer_info;
    MultisamplingInfo multisampling_info;
    ColorBlendInfo color_blend_info;
    DynamicInfo dynamic_info;

  public:

    explicit Vulkan(const MurderEngine* engine, const VulkanSurface &surface_instance);

    int compile_shader(Shader* shader) const override;

  protected:

    int initialize() override;
    int terminate() override;
    int tick(const Context context) override;

    /* physical device */
    int get_physical_device_queue_family(const VkPhysicalDevice physical_device,
	const int required_flags, uint32_t &family_index);
    int get_physical_device_extensions(const VkPhysicalDevice physical_device,
	const vector<const char*> &required_extensions, vector<VkExtensionProperties> &extensions);
    int get_physical_device_features(const VkPhysicalDevice physical_device,
	const vector<const char*> &required_features, VkPhysicalDeviceFeatures &features);
    int get_physical_device(const int required_flags,
	const vector<const char*> &required_extensions, const vector<const char*> &required_features, PhysicalDeviceInfo &physical_device_info);

    /* logical device */
    int create_device(VkDevice &device);

    /* command pool */
    int create_command_pool(VkCommandPool &command_pool);

    /* swapchain */
    int get_surface_format(const vector<VkSurfaceFormatKHR> &formats, VkSurfaceFormatKHR &format);
    int get_present_mode(const vector<VkPresentModeKHR> &modes, VkPresentModeKHR &mode);
    int get_extent(const Surface &surface_instance, const VkSurfaceCapabilitiesKHR surface_capabilities, VkExtent2D &extent);
    int create_swapchain(SwapchainInfo &swapchain_info);

    /* pipeline layout */
    int create_pipeline_layout(PipelineLayoutInfo &pipeline_layout_info);

    /* viewport */
    int create_viewports(ViewportInfo &viewport_info);

    /* rasterizer */
    int create_rasterizer(RasterizerInfo &rasterizer_info);

    /* multisampling */
    int create_multisampling(MultisamplingInfo &multisampling_info);

    /* color blending */
    int create_color_blend(ColorBlendInfo &color_blend_info);

    /* dynamic */
    int create_dynamic(DynamicInfo &dynamic_info);

  };

}

#endif
