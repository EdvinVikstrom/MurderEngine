#ifndef ME_VULKAN_HPP
  #define ME_VULKAN_HPP

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../memory/MemoryPool.hpp"

#include <vulkan/vulkan.hpp>

#include <set>

namespace me {

  class Vulkan : public Renderer {

  /* structs */
  protected:

    struct QueueFamilyIndices {
      std::optional<uint32_t> graphics;
    };
    
    struct PhysicalDeviceInfo {
      VkPhysicalDevice device;
      VkPhysicalDeviceProperties properties;
      VkPhysicalDeviceFeatures features;
      std::vector<VkExtensionProperties> extensions;
      QueueFamilyIndices queue_family_indices;
    };

  protected:

    Logger* logger;

    const Surface &surface;
    std::vector<const char*> extensions;

    VkInstance instance;
    PhysicalDeviceInfo physical_device_info = { VK_NULL_HANDLE };
    VkDevice device;
    VkCommandPool command_pool;

  public:

    explicit Vulkan(const MurderEngine* engine, const Surface &surface);

    int signal() override { return 0; }

    int initialize() override;
    int terminate() override;

    int tick() override;

  protected:

    /* physical device */
    int get_physical_device_queue_family(const VkPhysicalDevice physical_device,
	const int required_flags, uint32_t &family_index);
    int get_physical_device_extensions(const VkPhysicalDevice physical_device,
	const std::vector<const char*> &required_extensions, std::vector<VkExtensionProperties> &extensions);
    int get_physical_device_features(const VkPhysicalDevice physical_device,
	const std::vector<const char*> &required_features, VkPhysicalDeviceFeatures &features);
    int get_physical_device(const int required_flags,
	const std::vector<const char*> &required_extensions, const std::vector<const char*> &required_features, PhysicalDeviceInfo &physical_device_info);

    /* logical device */
    int create_device(const PhysicalDeviceInfo &physical_device_info, VkDevice &device);

    /* command pool */
    int create_command_pool(const PhysicalDeviceInfo &physical_device_info, const VkDevice device, VkCommandPool &command_pool);

  };

}

#endif
