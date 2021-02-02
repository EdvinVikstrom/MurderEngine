#include "Vulkan.hpp"
#include "Util.hpp"

static int create_image_view(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkImage 					image,
    VkFormat 					format,
    VkImageView 				&image_view
    );


int me::Vulkan::create_swapchain(const SwapchainCreateInfo &swapchain_create_info, Swapchain &swapchain)
{
  VERIFY_CREATE_INFO(swapchain_create_info, STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(swapchain_create_info.device)->vk_device;
  VkSurfaceKHR vk_surface = reinterpret_cast<Surface_T*>(swapchain_create_info.surface)->vk_surface;
  VkSurfaceCapabilitiesKHR vk_surface_capabilities = reinterpret_cast<Surface_T*>(swapchain_create_info.surface)->vk_capabilities;
  VkSurfaceFormatKHR vk_surface_format = reinterpret_cast<Surface_T*>(swapchain_create_info.surface)->vk_format;
  VkPresentModeKHR vk_present_mode = reinterpret_cast<Surface_T*>(swapchain_create_info.surface)->vk_present_mode;
  VkExtent2D vk_surface_extent = reinterpret_cast<Surface_T*>(swapchain_create_info.surface)->vk_extent;

  VkFormat vk_image_format = vk_surface_format.format;
  VkColorSpaceKHR vk_image_color_space = vk_surface_format.colorSpace;
  VkExtent2D vk_image_extent = vk_surface_extent;

  uint32_t min_image_count = math::min(vk_surface_capabilities.minImageCount + 1,
      vk_surface_capabilities.maxImageCount > 0 ? vk_surface_capabilities.maxImageCount : -1);

  VkSurfaceTransformFlagBitsKHR pre_transform = vk_surface_capabilities.currentTransform;
  VkPresentModeKHR present_mode = vk_present_mode;

  VkSwapchainCreateInfoKHR vk_swapchain_create_info = { };
  vk_swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  vk_swapchain_create_info.pNext = nullptr;
  vk_swapchain_create_info.flags = 0;
  vk_swapchain_create_info.surface = vk_surface;
  vk_swapchain_create_info.minImageCount = min_image_count;
  vk_swapchain_create_info.imageFormat = vk_image_format;
  vk_swapchain_create_info.imageColorSpace = vk_image_color_space;
  vk_swapchain_create_info.imageExtent = vk_image_extent;
  vk_swapchain_create_info.imageArrayLayers = 1;
  vk_swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (vk_surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (vk_surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  vk_swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vk_swapchain_create_info.queueFamilyIndexCount = 0;
  vk_swapchain_create_info.pQueueFamilyIndices = nullptr;
  vk_swapchain_create_info.preTransform = pre_transform;
  vk_swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  vk_swapchain_create_info.presentMode = present_mode;
  vk_swapchain_create_info.clipped = VK_TRUE;
  vk_swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; /* TODO */

  VkSwapchainKHR vk_swapchain;
  VkResult result = vkCreateSwapchainKHR(vk_device, &vk_swapchain_create_info, vk_allocation, &vk_swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", util::get_result_string(result));

  swapchain = alloc.allocate<Swapchain_T>(vk_swapchain, vk_image_format, vk_image_color_space, vk_image_extent);
  return 0;
}

int me::Vulkan::create_swapchain_images(const SwapchainImageCreateInfo &swapchain_image_create_info,
    uint32_t image_count, SwapchainImage* images)
{
  VERIFY_CREATE_INFO(swapchain_image_create_info, STRUCTURE_TYPE_SWAPCHAIN_IMAGE_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(swapchain_image_create_info.device)->vk_device;
  VkSwapchainKHR vk_swapchain = reinterpret_cast<Swapchain_T*>(swapchain_image_create_info.swapchain)->vk_swapchain;
  VkFormat vk_swapchain_image_format = reinterpret_cast<Swapchain_T*>(swapchain_image_create_info.swapchain)->vk_image_format;

  VkImage vk_images[image_count];
  VkResult result = vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, vk_images);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images [%s]", util::get_result_string(result));

  for (uint32_t i = 0; i < image_count; i++)
  {
    VkImageView vk_image_view;
    create_image_view(vk_device, vk_allocation, vk_images[i], vk_swapchain_image_format, vk_image_view);
    images[i] = alloc.allocate<SwapchainImage_T>(vk_images[i], vk_image_view, (VkFence) VK_NULL_HANDLE);
  }
  return 0;
}

int me::Vulkan::get_swapchain_image_count(Device device, Swapchain swapchain, uint32_t &image_count)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkSwapchainKHR vk_swapchain = reinterpret_cast<Swapchain_T*>(swapchain)->vk_swapchain;

  VkResult result = vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", util::get_result_string(result));
  return 0;
}

int me::Vulkan::cleanup_swapchain(Device device, Swapchain swapchain)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkSwapchainKHR vk_swapchain = reinterpret_cast<Swapchain_T*>(swapchain)->vk_swapchain;

  vkDestroySwapchainKHR(vk_device, vk_swapchain, vk_allocation);
  return 0;
}

int me::Vulkan::cleanup_swapchain_images(Device device, uint32_t image_count, SwapchainImage* images)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;

  for (uint32_t i = 0; i < image_count; i++)
  {
    VkImageView vk_image_view = reinterpret_cast<SwapchainImage_T*>(images[i])->vk_image_view;
    vkDestroyImageView(vk_device, vk_image_view, vk_allocation);
  }
  return 0;
}


int create_image_view(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkImage 					image,
    VkFormat 					format,
    VkImageView 				&image_view
    )
{
  VkImageViewCreateInfo image_view_create_info = { };
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.pNext = nullptr;
  image_view_create_info.flags = 0;
  image_view_create_info.image = image;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = format;
  image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  VkResult result = vkCreateImageView(device, &image_view_create_info, allocation, &image_view);
  if (result != VK_SUCCESS)
    throw me::exception("failed to create image view [%s]", me::util::get_result_string(result));
  return 0;
}
