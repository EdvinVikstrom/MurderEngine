#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../memory/MemoryPool.hpp"

#include <vulkan/vulkan.h>

#include <vulkan/vulkan_core.h>

#include <lme/optional.hpp>

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
      vector<VkImage> images;
      vector<VkImageView> image_views;
    };

    struct Storage {
      vector<VkShaderModule> shaders;
    };

  protected:

    Logger* logger;

    const Surface &surface_instance;
    vector<const char*> extensions;

    mutable Storage storage;

    VkInstance instance;
    VkSurfaceKHR surface;
    PhysicalDeviceInfo physical_device_info = { VK_NULL_HANDLE };
    VkDevice device;
    VkCommandPool command_pool;
    SurfaceInfo surface_info;
    SwapchainInfo swapchain_info;

  public:

    explicit Vulkan(const MurderEngine* engine, const Surface &surface_instance);

    int compile_shader(Shader* shader) const override;

  protected:

    int initialize() override;
    int terminate() override;

    int tick() override;

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

  };

}

#endif
