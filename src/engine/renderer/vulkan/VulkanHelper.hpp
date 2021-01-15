#ifndef ME_VULKAN_HELPER_HPP
  #define ME_VULKAN_HELPER_HPP

#include "Vulkan.hpp"

#include "../../util/vk_utils.hpp"

#include <memory.h>
#include <vulkan/vulkan_core.h>

int me::Vulkan::create_buffer(const VkPhysicalDevice physical_device,
    const VkDevice device,
    const VkDeviceSize buffer_size,
    const VkBufferUsageFlags buffer_usage_flags,
    const VkSharingMode sharing_mode,
    const VkMemoryPropertyFlags memory_property_flags,
    VkBuffer& buffer,
    VkDeviceMemory& buffer_memory)
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
    throw exception("failed to create vertex buffer [%s]", vk_utils_result_string(result));

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

int me::Vulkan::copy_buffer(const VkDevice device,
    const VkCommandPool command_pool,
    const VkQueue queue,
    const VkDeviceSize buffer_size,
    const VkBuffer source_buffer,
    const VkBuffer destination_buffer)
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

int me::Vulkan::map_buffer_memory(const VkDevice device,
    const VkDeviceSize buffer_size,
    const void* buffer_data,
    const VkDeviceMemory device_memory)
{
  void* data;
  VkResult result = vkMapMemory(device, device_memory, 0, buffer_size, 0, &data);
  if (result != VK_SUCCESS)
    throw exception("failed to map memory [%s]", vk_utils_result_string(result));

  /* use flush instead */
  memcpy(data, buffer_data, buffer_size);
  vkUnmapMemory(device, device_memory);
  return 0;
}

bool me::Vulkan::has_extensions(const array_proxy<VkExtensionProperties> &extension_properties,
    const vector<const char*> &required_extensions)
{
  for (const char* required_extension : required_extensions)
  {
    bool has_extension = false;
    for (uint32_t i = 0; i < extension_properties.size(); i++)
    {
      if (strcmp(required_extension, extension_properties[i].extensionName))
      {
	has_extension = true;
	break;
      }
    }

    if (!has_extension)
      return false;
  }
  return true;
}

bool me::Vulkan::has_layers(const array_proxy<VkLayerProperties> &layer_properties,
    const vector<const char*> &required_layers)
{
  for (const char* required : required_layers)
  {
    bool found = false;
    for (uint32_t i = 0; i < layer_properties.size(); i++)
    {
      if (strcmp(required, layer_properties[i].layerName) == 0)
      {
	found = true;
	break;
      }
    }

    if (!found)
      return false;
  }
  return true;
}

bool me::Vulkan::has_queue_families(const array_proxy<VkQueueFamilyProperties> &queue_family_properties,
    const vector<VkQueueFlags> &required_queue_family_properties)
{
  for (const uint32_t required : required_queue_family_properties)
  {
    bool found = false;
    for (uint32_t i = 0; i < queue_family_properties.size(); i++)
    {
      if ((queue_family_properties[i].queueFlags & required) == required)
      {
	found = true;
	break;
      }
    }

    if (!found)
      return false;
  }
  return true;
}

bool me::Vulkan::has_present_mode(const array_proxy<VkPresentModeKHR> &present_modes,
    const VkPresentModeKHR required_present_mode)
{
  for (uint32_t i = 0; i < present_modes.size(); i++)
  {
    if (required_present_mode == present_modes[i])
      return true;
  }
  return false;
}

int me::Vulkan::find_surface_format(const VkFormat color_format,
    const VkColorSpaceKHR color_space,
    const array_proxy<VkSurfaceFormatKHR> &surface_formats,
    VkSurfaceFormatKHR &surface_format)
{
  if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED)
  {
    surface_format = { color_format, color_space };
    return 0;
  }

  for (uint32_t i = 0; i < surface_formats.size(); i++)
  {
    if (surface_formats[i].format == color_format && surface_formats[i].colorSpace == color_space)
    {
      surface_format = surface_formats[i];
      return 0;
    }
  }

  if (surface_formats.size() > 0)
    surface_format = surface_formats[0];
  return 0;
}

int me::Vulkan::find_present_mode(const VkPresentModeKHR required_present_mode,
    const array_proxy<VkPresentModeKHR> &present_modes,
    VkPresentModeKHR &present_mode)
{
  if (has_present_mode(present_modes, required_present_mode))
    present_mode = required_present_mode;
  return 0;
}

int me::Vulkan::find_image_extent(const VkExtent2D &current_extent,
    VkExtent2D &image_extent)
{
  if (current_extent.width != UINT32_MAX)
    image_extent = current_extent;
  return 0;
}

int me::Vulkan::find_queue_families(const VkPhysicalDevice physical_device,
    const VkSurfaceKHR surface,
    const array_proxy<VkQueueFamilyProperties> &queue_family_properties,
    QueueFamilyIndices& queue_family_indices)
{

  for (uint32_t i = 0; i < queue_family_properties.size(); i++)
  {
    if (queue_family_properties[i].queueCount > 0)
    {
      if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	queue_family_indices.graphics.assign(i);

      /* TODO: multiple queue families */
      if (queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
	queue_family_indices.transfer.assign(i);

      VkBool32 present_support = VK_FALSE;
      vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);
      if (present_support)
	queue_family_indices.present.assign(i);
    }
  }
  return 0;
}

int me::Vulkan::get_extent(const VkExtent2D max_extent,
    const VkExtent2D min_extent,
    VkExtent2D &extent)
{
  /* if extent.width/height is more than max image width/height; set the extent.width/height to max image width/height */
  extent.width = math::min(extent.width, max_extent.width);
  extent.height = math::min(extent.height, max_extent.height);

  /* if extent.width/height is less than min image width/height; set the extent.width/height to min image width/height */
  extent.width = math::max(extent.width, min_extent.width);
  extent.height = math::max(extent.height, min_extent.height);
 
  return 0;
}

#endif
