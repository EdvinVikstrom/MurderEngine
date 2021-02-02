#include "Vulkan.hpp"
#include "Util.hpp"
#include "Memory.hpp"

int me::Vulkan::create_descriptor_pool(const DescriptorPoolCreateInfo &descriptor_pool_create_info, DescriptorPool &descriptor_pool)
{
  VERIFY_CREATE_INFO(descriptor_pool_create_info, STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(descriptor_pool_create_info.device)->vk_device;

  uint32_t pool_size_count = 1;
  VkDescriptorPoolSize vk_descriptor_pool_sizes[pool_size_count];
  vk_descriptor_pool_sizes[0].type = util::get_vulkan_descriptor_type(descriptor_pool_create_info.descriptor_type);
  vk_descriptor_pool_sizes[0].descriptorCount = descriptor_pool_create_info.descriptor_count;

  VkDescriptorPoolCreateInfo vk_descriptor_pool_create_info = {};
  vk_descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  vk_descriptor_pool_create_info.pNext = nullptr;
  vk_descriptor_pool_create_info.flags = 0;
  vk_descriptor_pool_create_info.maxSets = descriptor_pool_create_info.descriptor_count;
  vk_descriptor_pool_create_info.poolSizeCount = pool_size_count;
  vk_descriptor_pool_create_info.pPoolSizes = vk_descriptor_pool_sizes;

  VkDescriptorPool vk_descriptor_pool;
  VkResult result = vkCreateDescriptorPool(vk_device, &vk_descriptor_pool_create_info, vk_allocation, &vk_descriptor_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create descriptor pool [%s]", util::get_result_string(result));

  descriptor_pool = alloc.allocate<DescriptorPool_T>(vk_descriptor_pool, descriptor_pool_create_info.descriptor_type);
  return 0;
}

int me::Vulkan::create_command_pool(const CommandPoolCreateInfo &command_pool_create_info, CommandPool &command_pool)
{
  VERIFY_CREATE_INFO(command_pool_create_info, STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(command_pool_create_info.device)->vk_device;
  uint32_t queue_index = reinterpret_cast<Queue_T*>(command_pool_create_info.queue)->vk_index;

  VkCommandPoolCreateInfo vk_command_pool_create_info = { };
  vk_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  vk_command_pool_create_info.pNext = nullptr;
  vk_command_pool_create_info.flags = 0;
  vk_command_pool_create_info.queueFamilyIndex = queue_index;

  VkCommandPool vk_command_pool;
  VkResult result = vkCreateCommandPool(vk_device, &vk_command_pool_create_info, vk_allocation, &vk_command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", util::get_result_string(result));

  command_pool = alloc.allocate<CommandPool_T>(vk_command_pool);
  return 0;
}

int me::Vulkan::create_buffer(const BufferCreateInfo &buffer_create_info, Buffer &buffer)
{
  VERIFY_CREATE_INFO(buffer_create_info, STRUCTURE_TYPE_BUFFER_CREATE_INFO);

  VkPhysicalDevice vk_physical_device = reinterpret_cast<PhysicalDevice_T*>(buffer_create_info.physical_device)->vk_physical_device;
  VkDevice vk_device = reinterpret_cast<Device_T*>(buffer_create_info.device)->vk_device;

  VkBufferUsageFlags vk_buffer_usage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  VkMemoryPropertyFlags vk_memory_property = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
  if (buffer_create_info.usage == BUFFER_USAGE_STORAGE)
    vk_buffer_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  if (buffer_create_info.usage == BUFFER_USAGE_VERTEX_BUFFER)
    vk_buffer_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  if (buffer_create_info.usage == BUFFER_USAGE_INDEX_BUFFER)
    vk_buffer_usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  if (buffer_create_info.usage == BUFFER_USAGE_UNIFORM_BUFFER)
    vk_buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

  if (buffer_create_info.write_method == BUFFER_WRITE_METHOD_STANDARD)
  {
    vk_buffer_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    vk_memory_property = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  }else if (buffer_create_info.write_method == BUFFER_WRITE_METHOD_STAGING)
  {
    vk_buffer_usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vk_memory_property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  }

  VkDeviceSize vk_buffer_size = buffer_create_info.size;
  VkBuffer vk_buffer;
  VkDeviceMemory vk_buffer_memory;
  me::memory::create_buffer(vk_physical_device, vk_device, vk_buffer_size,
      vk_buffer_usage, VK_SHARING_MODE_EXCLUSIVE,
      vk_memory_property, vk_buffer, vk_buffer_memory);

  buffer = alloc.allocate<Buffer_T>(vk_buffer, vk_buffer_memory,
      buffer_create_info.usage, buffer_create_info.write_method, buffer_create_info.size);
  return 0;
}

int me::Vulkan::buffer_write(const BufferWriteInfo &buffer_write_info, Buffer buffer)
{
  VkPhysicalDevice vk_physical_device = reinterpret_cast<PhysicalDevice_T*>(buffer_write_info.physical_device)->vk_physical_device;
  VkDevice vk_device = reinterpret_cast<Device_T*>(buffer_write_info.device)->vk_device;
  VkBuffer vk_buffer = reinterpret_cast<Buffer_T*>(buffer)->vk_buffer;
  VkDeviceMemory vk_buffer_memory = reinterpret_cast<Buffer_T*>(buffer)->vk_memory;
  BufferWriteMethod buffer_write_method = reinterpret_cast<Buffer_T*>(buffer)->write_method;
  size_t buffer_size = reinterpret_cast<Buffer_T*>(buffer)->size;

  if (buffer_write_info.byte_count > buffer_size)
    throw exception("trying to write data to buffer larger than the buffer capacity. \e[31m%lu\e[0m > %lu",
	buffer_write_info.byte_count, buffer_size);

  VkDeviceSize vk_buffer_size = buffer_write_info.byte_count;

  if (buffer_write_method == BUFFER_WRITE_METHOD_STANDARD)
  {
    me::memory::map_buffer_memory(vk_device, vk_buffer_size, buffer_write_info.bytes, vk_buffer_memory);
  }else if (buffer_write_method == BUFFER_WRITE_METHOD_STAGING)
  {
    VkQueue vk_queue = reinterpret_cast<Queue_T*>(buffer_write_info.transfer_queue)->vk_queue;
    VkCommandPool vk_command_pool = reinterpret_cast<CommandPool_T*>(buffer_write_info.transfer_command_pool)->vk_command_pool;

    VkBuffer vk_staging_buffer;
    VkDeviceMemory vk_staging_buffer_memory;
    me::memory::create_buffer(vk_physical_device, vk_device, vk_buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vk_staging_buffer, vk_staging_buffer_memory);

    /* copy data to staging buffer */
    me::memory::map_buffer_memory(vk_device, vk_buffer_size, buffer_write_info.bytes, vk_staging_buffer_memory);

    /* copy buffer */
    me::memory::copy_buffer(vk_device, vk_command_pool, vk_queue, vk_buffer_size, vk_staging_buffer, vk_buffer);

    /* destroy and free */
    vkDestroyBuffer(vk_device, vk_staging_buffer, vk_allocation);
    vkFreeMemory(vk_device, vk_staging_buffer_memory, vk_allocation);
  }
  return 0;
}

int me::Vulkan::cleanup_descriptor_pool(Device device, DescriptorPool descriptor_pool)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkDescriptorPool vk_descriptor_pool = reinterpret_cast<DescriptorPool_T*>(descriptor_pool)->vk_descriptor_pool;

  vkDestroyDescriptorPool(vk_device, vk_descriptor_pool, vk_allocation);
  return 0;
}

int me::Vulkan::cleanup_command_pool(Device device, CommandPool command_pool)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkCommandPool vk_command_pool = reinterpret_cast<CommandPool_T*>(command_pool)->vk_command_pool;

  vkDestroyCommandPool(vk_device, vk_command_pool, vk_allocation);
  return 0;
}


int me::memory::create_buffer(
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

int me::memory::copy_buffer(
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

int me::memory::map_buffer_memory(
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

int me::memory::get_memory_type(
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

int me::memory::create_descriptor_pool(
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
    throw me::exception("failed to create descriptor pool [%s]", me::util::get_result_string(result));
  return 0;
}

int me::memory::create_command_pool(
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
    throw me::exception("failed to create command pool [%s]", me::util::get_result_string(result));
  return 0;
}
