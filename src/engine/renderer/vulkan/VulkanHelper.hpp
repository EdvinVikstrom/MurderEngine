#ifndef ME_VULKAN_HELPER_HPP
  #define ME_VULKAN_HELPER_HPP

#include "Vulkan.hpp"

bool me::Vulkan::has_required_extensions(const PhysicalDeviceInfo &physical_device_info, const vector<const char*> &required_extensions)
{
  for (const char* required_extension : required_extensions)
  {
    bool has_extension = false;
    for (uint32_t i = 0; i < physical_device_info.extensions.count; i++)
    {
      if (strcmp(physical_device_info.extensions[i].extensionName, required_extension))
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

bool me::Vulkan::has_present_mode(const uint32_t present_mode_count,
    const VkPresentModeKHR* present_modes,
    const VkPresentModeKHR present_mode)
{
  for (uint32_t i = 0; i < present_mode_count; i++)
  {
    if (present_mode == present_modes[i])
      return true;
  }
  return false;
}

int me::Vulkan::find_surface_format(const VkFormat color_format,
    const VkColorSpaceKHR color_space,
    const uint32_t format_count,
    const VkSurfaceFormatKHR* formats,
    VkSurfaceFormatKHR& format)
{
  if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
  {
    format = { color_format, color_space };
    return 0;
  }

  for (uint32_t i = 0; i < format_count; i++)
  {
    if (formats[i].format == color_format && formats[i].colorSpace == color_space)
    {
      format = formats[i];
      return 0;
    }
  }

  if (format_count > 0)
    format = formats[0];
  return 0;
}

int me::Vulkan::get_extension_names(uint32_t extension_count,
    const VkExtensionProperties* extensions,
    const char** extension_names)
{
  for (uint32_t i = 0; i < extension_count; i++)
    extension_names[i] = extensions[i].extensionName;
  return 0;
}

int me::Vulkan::get_surface_extent(const VkExtent2D max_extent,
    const VkExtent2D min_extent,
    VkExtent2D &extent)
{
  /* if extent.width/height is more than max image width/height; set the extent.width/height to max image width/height */
  extent.width = math::min(extent.width, max_extent.width);
  extent.height = math::min(extent.height, max_extent.height);

  /* if extent.width/height is less than min image width/height; set the extent.width/height to min image width/height */
  extent.width = math::max(extent.width, min_extent.width);
  extent.width = math::max(extent.height, min_extent.height);
 
  return 0;
}

int me::Vulkan::get_logical_device_queue_create_info(const uint32_t family_index,
    const uint32_t queue_count,
    const float* queue_priorities,
    VkDeviceQueueCreateInfo &device_create_info)
{
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.queueFamilyIndex = family_index;
  device_create_info.queueCount = queue_count;
  device_create_info.pQueuePriorities = queue_priorities;
  return 0;
}

#endif
