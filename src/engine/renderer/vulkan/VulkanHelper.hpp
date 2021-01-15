#ifndef ME_VULKAN_HELPER_HPP
  #define ME_VULKAN_HELPER_HPP

#include "Vulkan.hpp"

#include "../../util/vk_utils.hpp"

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
