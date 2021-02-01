#ifndef ME_RENDERER_VULKAN_TYPES_HPP
  #define ME_RENDERER_VULKAN_TYPES_HPP

#include "../Types.hpp"

#include <vulkan/vulkan.h>

#include <lme/vector.hpp>

namespace me {

  struct VulkanPhysicalDevice {
    VkPhysicalDevice vk_physical_device;
    VkPhysicalDeviceProperties vk_properties;
  };

  struct VulkanSurface {
    VkSurfaceKHR vk_surface;
    VkSurfaceCapabilitiesKHR vk_capabilities;
    VkSurfaceFormatKHR vk_format;
    VkPresentModeKHR vk_present_mode;
    VkExtent2D vk_extent;
  };

  struct VulkanDevice {
    VkDevice vk_device;
    uint32_t compute_queue_index;
    uint32_t graphics_queue_index;
    uint32_t present_queue_index;
    uint32_t transfer_queue_index;
  };

  struct VulkanQueue {
    VkQueue vk_queue;
    uint32_t vk_index;
  };

  struct VulkanSwapchain {
    VkSwapchainKHR vk_swapchain;
    VkFormat vk_image_format;
    VkColorSpaceKHR vk_image_color_space;
    VkExtent2D vk_image_extent;
  };

  struct VulkanSwapchainImage {
    VkImage vk_image;
    VkImageView vk_image_view;
    VkFence vk_in_flight_fence;
  };

  struct VulkanFrame {
    VkSemaphore vk_image_available_semaphore;
    VkSemaphore vk_render_finished_semaphore;
    VkFence vk_in_flight_fence;
  };

  struct VulkanBuffer {
    VkBuffer vk_buffer;
    VkDeviceMemory vk_memory;
    BufferUsage usage;
    BufferWriteMethod write_method;
    size_t size;
  };

  struct VulkanRenderPass {
    VkRenderPass vk_render_pass;
  };

  struct VulkanPipeline {
    VkPipeline vk_pipeline;
    VkPipelineLayout vk_layout;
    VkDescriptorSetLayout vk_descriptor_set_layout;
  };

  struct VulkanFramebuffer {
    VkFramebuffer vk_framebuffer;
  };

  struct VulkanDescriptorPool {
    VkDescriptorPool vk_descriptor_pool;
    DescriptorType type;
  };

  struct VulkanDescriptor {
    VkDescriptorSet vk_descriptor_set;
  };

  struct VulkanCommandPool {
    VkCommandPool vk_command_pool;
  };

  struct VulkanCommandBuffer {
    VkCommandBuffer vk_command_buffer;
    CommandBufferUsage usage;
  };

}

#endif
