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


int me::Vulkan::create_surface(const SurfaceCreateInfo &surface_create_info, Surface &surface)
{
  VERIFY_CREATE_INFO(surface_create_info, STRUCTURE_TYPE_SURFACE_CREATE_INFO);

  VkPhysicalDevice vk_physical_device = reinterpret_cast<VulkanPhysicalDevice*>(surface_create_info.physical_device)->vk_physical_device;

  VkSurfaceKHR vk_surface;
  surface_create_info.surface_module->vk_create_surface(vk_instance, vk_allocation, &vk_surface);

  VkSurfaceCapabilitiesKHR vk_surface_capabilities;
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
  VkPresentModeKHR vk_present_mode;
  if (algorithm::array_find({present_mode_count, present_modes}, VK_PRESENT_MODE_MAILBOX_KHR) != SIZE_MAX)
    vk_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
  else
    vk_present_mode = VK_PRESENT_MODE_FIFO_KHR;

  VkExtent2D vk_extent;
  surface_create_info.surface_module->get_framebuffer_size(vk_extent.width, vk_extent.height);
  get_extent(vk_surface_capabilities, vk_extent);

  VkSurfaceFormatKHR vk_surface_format;
  get_format(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, {surface_format_count, surface_formats}, vk_surface_format);

  surface = alloc.allocate<VulkanSurface>(vk_surface, vk_surface_capabilities, vk_surface_format, vk_present_mode, vk_extent);
  return 0;
}

int me::Vulkan::cleanup_surface(const SurfaceCleanupInfo &surface_cleanup_info, Surface surface)
{
  VulkanSurface* vulkan_surface = reinterpret_cast<VulkanSurface*>(surface);

  vkDestroySurfaceKHR(vk_instance, vulkan_surface->vk_surface, vk_allocation);
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
