#include "Vulkan.hpp"
#include "Util.hpp"

#include <lme/algorithm.hpp>
#include <lme/math/math.hpp>

static int get_format(
    const VkFormat 						color_format,
    const VkColorSpaceKHR 					color_space,
    const me::array_proxy<VkSurfaceFormatKHR, uint32_t> 	&surface_formats,
    VkSurfaceFormatKHR 						&surface_format
    );

static int get_extent(
    const VkSurfaceCapabilitiesKHR 				&surface_capabilities,
    VkExtent2D 							&image_extent
    );


int me::vulkan::Vulkan::setup_surface(const SurfaceInfo &surface_info)
{
  surface_info.surface->register_resize_surface_callback(surface_resize_callback, this);

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, vk_surface, &vk_surface_capabilities);

  /* getting surface formats */
  uint32_t surface_format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &surface_format_count, nullptr);
  VkSurfaceFormatKHR surface_formats[surface_format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &surface_format_count, surface_formats);

  /* getting present modes */
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &present_mode_count, nullptr);
  VkPresentModeKHR present_modes[present_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &present_mode_count, present_modes);

  /* checking if VK_PRESENT_MODE_MAILBOX_KHR is supported */
  if (algorithm::array_find({present_mode_count, present_modes}, VK_PRESENT_MODE_MAILBOX_KHR) != SIZE_MAX)
    vk_surface_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
  else
    vk_surface_present_mode = VK_PRESENT_MODE_FIFO_KHR;

  surface_info.surface->get_framebuffer_size(vk_surface_extent.width, vk_surface_extent.height);
  get_format(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, {surface_format_count, surface_formats}, vk_surface_format);
  get_extent(vk_surface_capabilities, vk_surface_extent);
  return 0;
}

int me::vulkan::Vulkan::surface_resize_callback(
    uint32_t 							width,
    uint32_t 							height,
    void* 							ptr
    )
{
  Vulkan* instance = reinterpret_cast<Vulkan*>(ptr);
  instance->render_info.flags |= RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
  return 0;
}

int me::vulkan::Vulkan::cleanup_surface()
{
  vkDestroySurfaceKHR(vk_instance, vk_surface, vk_allocation);
  return 0;
}


int get_format(
    const VkFormat 						color_format,
    const VkColorSpaceKHR 					color_space,
    const me::array_proxy<VkSurfaceFormatKHR, uint32_t> 	&surface_formats,
    VkSurfaceFormatKHR 						&surface_format
    )
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

int get_extent(
    const VkSurfaceCapabilitiesKHR 				&surface_capabilities,
    VkExtent2D 							&image_extent
    )
{
  if (surface_capabilities.currentExtent.width != UINT32_MAX)
    image_extent = surface_capabilities.currentExtent;

  /* if extent.width/height is more than max image width/height; set the extent.width/height to max image width/height */
  image_extent.width = me::math::min(image_extent.width, surface_capabilities.maxImageExtent.width);
  image_extent.height = me::math::min(image_extent.height, surface_capabilities.maxImageExtent.height);

  /* if extent.width/height is less than min image width/height; set the extent.width/height to min image width/height */
  image_extent.width = me::math::max(image_extent.width, surface_capabilities.minImageExtent.width);
  image_extent.height = me::math::max(image_extent.height, surface_capabilities.minImageExtent.height);
  return 0;
}
