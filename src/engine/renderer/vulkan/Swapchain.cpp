#include "Vulkan.hpp"
#include "Util.hpp"

static int create_image_view(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkImage 					image,
    VkFormat 					format,
    VkImageView 				&image_view
    );


int me::vulkan::Vulkan::setup_swapchain(const SwapchainInfo &swapchain_info, Swapchain &_swapchain)
{
  vk_swapchain_image_format = vk_surface_format.format;
  vk_swapchain_image_color_space = vk_surface_format.colorSpace;
  vk_swapchain_image_extent = vk_surface_extent;

  uint32_t min_image_count = math::min(vk_surface_capabilities.minImageCount + 1,
      vk_surface_capabilities.maxImageCount > 0 ? vk_surface_capabilities.maxImageCount : -1);

  VkSurfaceTransformFlagBitsKHR pre_transform = vk_surface_capabilities.currentTransform;
  VkPresentModeKHR present_mode = vk_surface_present_mode;

  VkSwapchainCreateInfoKHR swapchain_create_info = { };
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = nullptr;
  swapchain_create_info.flags = 0;
  swapchain_create_info.surface = vk_surface;
  swapchain_create_info.minImageCount = min_image_count;
  swapchain_create_info.imageFormat = vk_swapchain_image_format;
  swapchain_create_info.imageColorSpace = vk_swapchain_image_color_space;
  swapchain_create_info.imageExtent = vk_swapchain_image_extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (vk_surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (vk_surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.preTransform = pre_transform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = present_mode;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; /* TODO */

  VkResult result = vkCreateSwapchainKHR(vk_device, &swapchain_create_info, vk_allocation, &vk_swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", util::get_result_string(result));

  /* get swapchain images */
  uint32_t image_count;
  result = vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", util::get_result_string(result));

  swapchain_images.resize(image_count);
  VkImage images[image_count];
  result = vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, images);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images [%s]", util::get_result_string(result));

  /* creating swapchain image views */
  for (uint32_t i = 0; i < image_count; i++)
  {
    swapchain_images[i].image = images[i];
    create_image_view(vk_device, vk_allocation, swapchain_images[i].image, vk_surface_format.format, swapchain_images[i].image_view);
  }

  _swapchain = 0;
  return 0;
}

int me::vulkan::Vulkan::cleanup_swapchain()
{
  for (uint32_t i = 0; i < swapchain_images.size(); i++)
    vkDestroyImageView(vk_device, swapchain_images[i].image_view, vk_allocation);
  vkDestroySwapchainKHR(vk_device, vk_swapchain, vk_allocation);
  return 0;
}

int me::vulkan::Vulkan::refresh_swapchain()
{
  /*
  uint32_t width, height;
  me_surface->get_framebuffer_size(width, height);
  if (width == 0 && height == 0)
  {
    render_info.state = RenderInfo::NO_SWAPCHAIN_STATE;
    return 1;
  }

  //call_callback_framebuffer_resize(width, height);

  vkDeviceWaitIdle(vk_device);

  cleanup_framebuffers();
  cleanup_graphics_pipeline();
  cleanup_pipeline_layout();
  cleanup_render_pass();
  cleanup_image_views();
  cleanup_swapchain();
  cleanup_descriptor_pool();

  setup_swapchain();
  setup_image_views();
  setup_render_pass();
  setup_shaders();
  setup_graphics_pipeline();
  setup_framebuffers();
  setup_uniform_buffers();
  setup_descriptor_pool();
  setup_descriptor_sets();
  setup_command_buffers();
  */
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
    throw me::exception("failed to create image view [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}
