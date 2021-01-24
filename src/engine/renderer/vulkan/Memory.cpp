#include "Vulkan.hpp"
#include "Util.hpp"

static int create_descriptor_pool(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    uint32_t 					count,
    VkDescriptorPool 				&descriptor_pool
    );

static int create_command_pool(
    VkDevice 					device,
    VkAllocationCallbacks*			allocation,
    uint32_t 					queue_family_index,
    VkCommandPool 				&command_pool
    );


int me::vulkan::Vulkan::setup_memory(const MemoryInfo &memory_info, Memory &_memory)
{
  vk_image_available_semaphores.resize(RenderInfo::MAX_FRAMES_IN_FLIGHT);
  vk_render_finished_semaphores.resize(RenderInfo::MAX_FRAMES_IN_FLIGHT);
  vk_in_flight_fences.resize(RenderInfo::MAX_FRAMES_IN_FLIGHT);
  vk_images_in_flight_fences.resize(swapchain_images.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_create_info = { };
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = { };
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < RenderInfo::MAX_FRAMES_IN_FLIGHT; i++)
  {
    /* create 'image available' semaphore */
    VkResult result = vkCreateSemaphore(vk_device, &semaphore_create_info, vk_allocation, &vk_image_available_semaphores[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", util::get_result_string(result));

    /* create 'render finished' semaphore */
    result = vkCreateSemaphore(vk_device, &semaphore_create_info, vk_allocation, &vk_render_finished_semaphores[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", util::get_result_string(result));

    /* create 'in flight' fence */
    result = vkCreateFence(vk_device, &fence_create_info, vk_allocation, &vk_in_flight_fences[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create fence [%s]", util::get_result_string(result));
  }

  create_descriptor_pool(vk_device, vk_allocation, swapchain_images.size(), vk_descriptor_pool);
  create_command_pool(vk_device, vk_allocation, queue_family_indices.graphics_queue, vk_graphics_command_pool);
  create_command_pool(vk_device, vk_allocation, queue_family_indices.transfer_queue, vk_transfer_command_pool);

  _memory = 0;
  return 0;
}

int me::vulkan::Vulkan::cleanup_memory()
{
  vkFreeMemory(vk_device, vk_vertex_buffer_memory, vk_allocation);
  vkFreeMemory(vk_device, vk_index_buffer_memory, vk_allocation);
  for (uint32_t i = 0; i < vk_uniform_buffers_memory.size(); i++)
    vkFreeMemory(vk_device, vk_uniform_buffers_memory[i], vk_allocation);

  for (uint32_t i = 0; i < RenderInfo::MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroySemaphore(vk_device, vk_image_available_semaphores[i], vk_allocation);
    vkDestroySemaphore(vk_device, vk_render_finished_semaphores[i], vk_allocation);
    vkDestroyFence(vk_device, vk_in_flight_fences[i], vk_allocation);
  }
  return 0;
}


#include "Memory.hpp"

int me::vulkan::memory::create_buffer(
    VkPhysicalDevice 					physical_device,
    VkDevice 						device,
    VkDeviceSize 					buffer_size,
    VkBufferUsageFlags 					buffer_usage_flags,
    VkSharingMode 					sharing_mode,
    VkMemoryPropertyFlags 				memory_property_flags,
    VkBuffer 						&buffer,
    VkDeviceMemory 					&buffer_memory
    )
{
  VkBufferCreateInfo buffer_create_info = { };
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.pNext = nullptr;
  buffer_create_info.flags = 0;
  buffer_create_info.size = buffer_size;
  buffer_create_info.usage = buffer_usage_flags;
  buffer_create_info.sharingMode = sharing_mode;
  buffer_create_info.queueFamilyIndexCount = 0;
  buffer_create_info.pQueueFamilyIndices = nullptr;

  VkResult result = vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer);
  if (result != VK_SUCCESS)
    throw exception("failed to create vertex buffer [%s]", util::get_result_string(result));

  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

  uint32_t memory_type;
  get_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_property_flags, memory_type);

  VkMemoryAllocateInfo buffer_memory_allocate_info = { };
  buffer_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  buffer_memory_allocate_info.pNext = nullptr;
  buffer_memory_allocate_info.allocationSize = memory_requirements.size;
  buffer_memory_allocate_info.memoryTypeIndex = memory_type;

  result = vkAllocateMemory(device, &buffer_memory_allocate_info, nullptr, &buffer_memory);
  if (result != VK_SUCCESS)
    throw exception("failed to allocate memory with mesh vertices(%lu) [%s]", buffer_size, result);

  /* param[3]: offset */
  vkBindBufferMemory(device, buffer, buffer_memory, 0);
  return 0;
}

int me::vulkan::memory::copy_buffer(
    VkDevice 						device,
    VkCommandPool 					command_pool,
    VkQueue 						queue,
    VkDeviceSize 					buffer_size,
    VkBuffer 						source_buffer,
    VkBuffer 						destination_buffer
    )
{
  VkCommandBufferAllocateInfo command_buffer_allocate_info = { };
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.pNext = nullptr;
  command_buffer_allocate_info.commandPool = command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = 1;

  VkCommandBuffer command_buffer;
  vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);

  VkCommandBufferBeginInfo command_buffer_begin_info = { };
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.pNext = nullptr;
  command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  command_buffer_begin_info.pInheritanceInfo = nullptr;

  /* begin recording */
  vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

  VkBufferCopy buffer_copy_region = { };
  buffer_copy_region.srcOffset = 0;
  buffer_copy_region.dstOffset = 0;
  buffer_copy_region.size = buffer_size;

  vkCmdCopyBuffer(command_buffer, source_buffer, destination_buffer, 1, &buffer_copy_region);

  /* stop recording */
  vkEndCommandBuffer(command_buffer);

  /* execute copy command */
  uint32_t submit_info_count = 1;
  VkSubmitInfo submit_infos[submit_info_count];
  submit_infos[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_infos[0].pNext = nullptr;
  submit_infos[0].waitSemaphoreCount = 0;
  submit_infos[0].pWaitSemaphores = nullptr;
  submit_infos[0].commandBufferCount = 1;
  submit_infos[0].pCommandBuffers = &command_buffer;
  submit_infos[0].signalSemaphoreCount = 0;
  submit_infos[0].pSignalSemaphores = nullptr;

  vkQueueSubmit(queue, submit_info_count, submit_infos, VK_NULL_HANDLE);
  vkQueueWaitIdle(queue);

  vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
  return 0;
}

int me::vulkan::memory::map_buffer_memory(
    VkDevice 						device,
    VkDeviceSize 					buffer_size,
    void* 						buffer_data,
    VkDeviceMemory 					device_memory
    )
{
  void* data;
  VkResult result = vkMapMemory(device, device_memory, 0, buffer_size, 0, &data);
  if (result != VK_SUCCESS)
    throw exception("failed to map memory [%s]", util::get_result_string(result));

  /* TODO: use flush instead */
  memcpy(data, buffer_data, (size_t) buffer_size);
  vkUnmapMemory(device, device_memory);
  return 0;
}

int me::vulkan::memory::get_memory_type(
    VkPhysicalDevice 					physical_device,
    uint32_t 						type_filter,
    VkMemoryPropertyFlags 				memory_property_flags,
    uint32_t 						&memory_type
    )
{
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

  for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
  {
    if (type_filter & (1 << i) &&
	(physical_device_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
    {
      memory_type = i;
      return 0;
    }
  }
  throw exception("failed to find suitable memory type");
}


int create_descriptor_pool(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    uint32_t 					count,
    VkDescriptorPool 				&descriptor_pool
    )
{
  uint32_t descriptor_pool_size_count = 1;
  VkDescriptorPoolSize descriptor_pool_sizes[descriptor_pool_size_count];
  descriptor_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptor_pool_sizes[0].descriptorCount = count;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info = { };
  descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create_info.pNext = nullptr;
  descriptor_pool_create_info.flags = 0;
  descriptor_pool_create_info.maxSets = count;
  descriptor_pool_create_info.poolSizeCount = descriptor_pool_size_count;
  descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes;

  VkResult result = vkCreateDescriptorPool(device, &descriptor_pool_create_info, allocation, &descriptor_pool);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create descriptor pool [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}

int create_command_pool(
    VkDevice 					device,
    VkAllocationCallbacks*			allocation,
    uint32_t 					queue_family_index,
    VkCommandPool 				&command_pool
    )
{
  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = 0;
  command_pool_create_info.queueFamilyIndex = queue_family_index;
 
  VkResult result = vkCreateCommandPool(device, &command_pool_create_info, allocation, &command_pool);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create command pool [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}
