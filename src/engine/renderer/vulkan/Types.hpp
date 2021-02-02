#ifndef ME_RENDERER_VULKAN_TYPES_HPP
  #define ME_RENDERER_VULKAN_TYPES_HPP

#include "../Types.hpp"

#include <vulkan/vulkan.h>

#include <lme/vector.hpp>

namespace me {

  struct PhysicalDevice_T {
    VkPhysicalDevice vk_physical_device;
    VkPhysicalDeviceProperties vk_properties;
    VkPhysicalDeviceFeatures vk_features;
  };

  struct Surface_T {
    VkSurfaceKHR vk_surface;
    VkSurfaceCapabilitiesKHR vk_capabilities;
    VkSurfaceFormatKHR vk_format;
    VkPresentModeKHR vk_present_mode;
    VkExtent2D vk_extent;
  };

  struct Device_T {
    VkDevice vk_device;
    uint32_t compute_queue_index;
    uint32_t graphics_queue_index;
    uint32_t present_queue_index;
    uint32_t transfer_queue_index;
  };

  struct Queue_T {
    VkQueue vk_queue;
    uint32_t vk_index;
  };

  struct Swapchain_T {
    VkSwapchainKHR vk_swapchain;
    VkFormat vk_image_format;
    VkColorSpaceKHR vk_image_color_space;
    VkExtent2D vk_image_extent;
  };

  struct SwapchainImage_T {
    VkImage vk_image;
    VkImageView vk_image_view;
    VkFence vk_in_flight_fence;
  };

  struct Frame_T {
    VkSemaphore vk_image_available_semaphore;
    VkSemaphore vk_render_finished_semaphore;
    VkFence vk_in_flight_fence;
  };

  struct Buffer_T {
    VkBuffer vk_buffer;
    VkDeviceMemory vk_memory;
    BufferUsage usage;
    BufferWriteMethod write_method;
    size_t size;
  };

  struct RenderPass_T {
    VkRenderPass vk_render_pass;
  };

  struct Pipeline_T {
    VkPipeline vk_pipeline;
    VkPipelineLayout vk_layout;
    VkDescriptorSetLayout vk_descriptor_set_layout;
  };

  struct Framebuffer_T {
    VkFramebuffer vk_framebuffer;
  };

  struct DescriptorPool_T {
    VkDescriptorPool vk_descriptor_pool;
    DescriptorType type;
  };

  struct Descriptor_T {
    VkDescriptorSet vk_descriptor_set;
  };

  struct CommandPool_T {
    VkCommandPool vk_command_pool;
  };

  struct CommandBuffer_T {
    VkCommandBuffer vk_command_buffer;
    CommandBufferUsage usage;
  };

}

#endif
