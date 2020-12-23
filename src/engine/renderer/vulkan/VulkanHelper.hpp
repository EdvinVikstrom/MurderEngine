#ifndef ME_VULKAN_HELPER_HPP
  #define ME_VULKAN_HELPER_HPP

#include "Vulkan.hpp"

bool me::Vulkan::has_extensions(const uint32_t extension_property_count,
    const VkExtensionProperties* extension_properties,
    const vector<const char*> &required_extensions)
{
  for (const char* required_extension : required_extensions)
  {
    bool has_extension = false;
    for (uint32_t i = 0; i < extension_property_count; i++)
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

bool me::Vulkan::has_layers(const uint32_t layer_property_count,
    const VkLayerProperties* layer_properties,
    const vector<const char*> &required_layers)
{
  for (const char* required : required_layers)
  {
    bool found = false;
    for (uint32_t i = 0; i < layer_property_count; i++)
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

bool me::Vulkan::has_queue_families(const uint32_t queue_family_property_count,
    const VkQueueFamilyProperties* queue_family_properties,
    const vector<VkQueueFlags> &required_queue_family_properties)
{
  for (const uint32_t required : required_queue_family_properties)
  {
    bool found = false;
    for (uint32_t i = 0; i < queue_family_property_count; i++)
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

int me::Vulkan::find_queue_families(const VkPhysicalDevice physical_device,
    const VkSurfaceKHR surface,
    const uint32_t queue_family_property_count,
    const VkQueueFamilyProperties* queue_family_properties,
    QueueFamilyIndices& queue_family_indices)
{

  for (uint32_t i = 0; i < queue_family_property_count; i++)
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

int me::Vulkan::get_shader_stage_flag(const ShaderType shader_type,
    VkShaderStageFlagBits &shader_stage_flag_bits)
{
  switch (shader_type)
  {
    case SHADER_VERTEX:
      shader_stage_flag_bits = VK_SHADER_STAGE_VERTEX_BIT;
      break;
    case SHADER_FRAGMENT:
      shader_stage_flag_bits = VK_SHADER_STAGE_FRAGMENT_BIT;
      break;
    case SHADER_GEOMETRY:
      shader_stage_flag_bits = VK_SHADER_STAGE_GEOMETRY_BIT;
      break;
  }
  return 0;
}

int me::Vulkan::get_logical_device_queue_create_info(const uint32_t family_index,
    const uint32_t queue_count,
    const float* queue_priorities,
    VkDeviceQueueCreateInfo &device_create_info)
{
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  device_create_info.pNext = nullptr;
  device_create_info.flags = 0;
  device_create_info.queueFamilyIndex = family_index;
  device_create_info.queueCount = queue_count;
  device_create_info.pQueuePriorities = queue_priorities;
  return 0;
}

#endif
