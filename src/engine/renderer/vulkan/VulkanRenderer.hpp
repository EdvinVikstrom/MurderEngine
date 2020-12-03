#ifndef ME_HADES_RENDERER_HPP
  #define ME_HADES_RENDERER_HPP

#include "../Renderer.hpp"
#include "../../surface/Surface.hpp"
#include "../../memory/MemoryPool.hpp"

#include <vulkan/vulkan.hpp>

#include <set>

namespace me {

  class VulkanRenderer : public Renderer {

  public:

    struct Features {
    };

  protected:

    Logger* logger;

    const Surface &surface;
    std::vector<const char*> extensions;

    MemPool<void> memory;

    struct Config {
      bool use_protected_memory = true;
    } config;

    VkAllocationCallbacks alloc_callbacks;
    VkInstance instance;
    
    struct QueueFamilyIndices {
      std::optional<uint32_t> graphics_family;
      std::optional<uint32_t> compute_family;
      std::optional<uint32_t> transfer_family;
    };

    struct Queues {
      std::vector<VkQueue> graphics_queues;
    };

    std::vector<VkPhysicalDeviceGroupProperties> physical_devices_group_properties;

    VkPhysicalDevice physical_device;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceFeatures physical_device_features;
    QueueFamilyIndices queue_family_indices;
    VkDevice device;
    Queues queues;
    VkCommandPool command_pool;


  public:

    explicit VulkanRenderer(const MurderEngine* engine, const Surface &surface);

    int vk_error(const VkResult result);

    int signal() override;

    int initialize() override;
    int terminate() override;

    int tick() override;

  protected:

    VkPhysicalDevice& get_primary_device(uint32_t count, VkPhysicalDevice* devices);
    int init_devices();
    int free_devices();

    int init_commands();
    int free_command_buffers();

    static void* vk_allocate(void* user_data, size_t size, size_t alignment, VkSystemAllocationScope scope);
    static void* vk_reallocate(void* user_data, void* original, size_t size, size_t alignment, VkSystemAllocationScope scope);
    static void vk_free(void* user_data, void* memory);
    static void vk_iallocaten(void* user_data, size_t size, VkInternalAllocationType alloc_type, VkSystemAllocationScope scope);
    static void vk_ifreen(void* user_data, size_t size, VkInternalAllocationType alloc_type, VkSystemAllocationScope scope);

  };

}

#endif
