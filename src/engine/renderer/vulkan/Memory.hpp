#ifndef ME_VULKAN_MEMORY_HPP
  #define ME_VULKAN_MEMORY_HPP

#include <vulkan/vulkan.h>

namespace me::vulkan::memory {

  int create_buffer(
      VkPhysicalDevice 					physical_device,
      VkDevice 						device,
      VkDeviceSize 					buffer_size,
      VkBufferUsageFlags 				buffer_usage_flags,
      VkSharingMode 					sharing_mode,
      VkMemoryPropertyFlags 				memory_property_flags,
      VkBuffer 						&buffer,
      VkDeviceMemory 					&buffer_memory
      );

  int copy_buffer(
      VkDevice 						device,
      VkCommandPool 					command_pool,
      VkQueue 						queue,
      VkDeviceSize 					buffer_size,
      VkBuffer 						source_buffer,
      VkBuffer 						destination_buffer
      );

  int map_buffer_memory(
      VkDevice 						device,
      VkDeviceSize 					buffer_size,
      void* 						buffer_data,
      VkDeviceMemory 					device_memory
      );

  int get_memory_type(
      VkPhysicalDevice 					physical_device,
      uint32_t 						type_filter,
      VkMemoryPropertyFlags 				memory_property_flags,
      uint32_t 						&memory_type
      );

}

#endif
