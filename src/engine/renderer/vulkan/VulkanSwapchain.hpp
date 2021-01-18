#include "Vulkan.inc"

/* <--- SETUP ---> */
int me::Vulkan::setup_surface()
{
  me_surface->vk_create_surface(instance_info.instance, nullptr, &surface_info.surface);
  me_surface->register_resize_surface_callback(callback_surface_resize, this);
  return 0;
}

int me::Vulkan::setup_swapchain()
{
  logger.debug("> SETUP_SWAPCHAIN");

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device_info.device, surface_info.surface, &swapchain_info.surface_capabilities);

  /* get surface formats */
  uint32_t surface_format_count;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface_info.surface, &surface_format_count, nullptr);
  VkSurfaceFormatKHR surface_formats[surface_format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device_info.device, surface_info.surface, &surface_format_count, surface_formats);

  /* get present modes */
  uint32_t present_mode_count;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface_info.surface, &present_mode_count, nullptr);
  VkPresentModeKHR present_modes[present_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device_info.device, surface_info.surface, &present_mode_count, present_modes);
  swapchain_info.present_mode = VK_PRESENT_MODE_FIFO_KHR;

  find_surface_format(VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, {surface_format_count, surface_formats}, swapchain_info.surface_format);
  find_present_mode(VK_PRESENT_MODE_MAILBOX_KHR, {present_mode_count, present_modes}, swapchain_info.present_mode);
  find_image_extent(swapchain_info.surface_capabilities.currentExtent, swapchain_info.image_extent);
  me_surface->get_framebuffer_size(swapchain_info.image_extent.width, swapchain_info.image_extent.height);
  get_extent(swapchain_info.surface_capabilities.maxImageExtent, swapchain_info.surface_capabilities.minImageExtent, swapchain_info.image_extent);


  swapchain_info.image_format = swapchain_info.surface_format.format;
  swapchain_info.image_color_space = swapchain_info.surface_format.colorSpace;
  swapchain_info.image_extent = swapchain_info.image_extent;

  uint32_t min_image_count = math::min(swapchain_info.surface_capabilities.minImageCount + 1,
      swapchain_info.surface_capabilities.maxImageCount > 0 ? swapchain_info.surface_capabilities.maxImageCount : -1);
  VkSurfaceTransformFlagBitsKHR pre_transform = swapchain_info.surface_capabilities.currentTransform;
  VkPresentModeKHR present_mode = swapchain_info.present_mode;

  VkSwapchainCreateInfoKHR swapchain_create_info = { };
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = nullptr;
  swapchain_create_info.flags = 0;
  swapchain_create_info.surface = surface_info.surface;
  swapchain_create_info.minImageCount = min_image_count;
  swapchain_create_info.imageFormat = swapchain_info.image_format;
  swapchain_create_info.imageColorSpace = swapchain_info.image_color_space;
  swapchain_create_info.imageExtent = swapchain_info.image_extent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | (
    (swapchain_info.surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) |
    (swapchain_info.surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = nullptr;
  swapchain_create_info.preTransform = pre_transform;
  swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_create_info.presentMode = present_mode;
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.oldSwapchain = VK_NULL_HANDLE; /* TODO */

  VkResult result = vkCreateSwapchainKHR(logical_device_info.device, &swapchain_create_info, nullptr, &swapchain_info.swapchain);
  if (result != VK_SUCCESS)
    throw exception("failed to create swapchain [%s]", vk_utils_result_string(result));

  /* get swapchain images */
  uint32_t image_count;
  result = vkGetSwapchainImagesKHR(logical_device_info.device, swapchain_info.swapchain, &image_count, nullptr);
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images count [%s]", vk_utils_result_string(result));

  swapchain_info.images.resize(image_count);
  result = vkGetSwapchainImagesKHR(logical_device_info.device, swapchain_info.swapchain, &image_count, swapchain_info.images.data());
  if (result != VK_SUCCESS)
    throw exception("failed to get swapchain images [%s]", vk_utils_result_string(result));
 return 0;
}

int me::Vulkan::setup_framebuffers()
{
  logger.debug("> SETUP_FRAMEBUFFERS");

  framebuffer_info.framebuffers.resize(swapchain_info.image_views.size());

  for (uint32_t i = 0; i < swapchain_info.image_views.size(); i++)
  {
    uint32_t attachment_count = 1;
    VkImageView attachments[attachment_count];
    attachments[0] = swapchain_info.image_views[i];

    VkFramebufferCreateInfo framebuffer_create_info = { };
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.pNext = nullptr;
    framebuffer_create_info.flags = 0;
    framebuffer_create_info.renderPass = render_pass_info.render_pass;
    framebuffer_create_info.attachmentCount = attachment_count;
    framebuffer_create_info.pAttachments = attachments;
    framebuffer_create_info.width = swapchain_info.image_extent.width;
    framebuffer_create_info.height = swapchain_info.image_extent.height;
    framebuffer_create_info.layers = 1;

    VkResult result = vkCreateFramebuffer(logical_device_info.device, &framebuffer_create_info, nullptr, &framebuffer_info.framebuffers[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create framebuffer [%s]", vk_utils_result_string(result));
  }
  return 0;
}


/* <--- OTHER ---> */
int me::Vulkan::refresh_swapchains()
{
  uint32_t width, height;
  me_surface->get_framebuffer_size(width, height);
  if (width == 0 && height == 0)
  {
    render_info.state = RenderInfo::NO_SWAPCHAIN_STATE;
    return 1;
  }

  vkDeviceWaitIdle(logical_device_info.device);

  cleanup_framebuffers();
  cleanup_graphics_pipeline();
  cleanup_pipeline_layout();
  cleanup_render_pass();
  cleanup_image_views();
  cleanup_swapchain();

  setup_swapchain();
  setup_image_views();
  setup_render_pass();
  setup_shaders();
  setup_pipeline_layout();
  setup_graphics_pipeline();
  setup_framebuffers();
  setup_command_buffers();
  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_surface()
{
  vkDestroySurfaceKHR(instance_info.instance, surface_info.surface, nullptr);
  return 0;
}

int me::Vulkan::cleanup_swapchain()
{
  vkDestroySwapchainKHR(logical_device_info.device, swapchain_info.swapchain, nullptr);
  return 0;
}

int me::Vulkan::cleanup_framebuffers()
{
  for (uint32_t i = 0; i < framebuffer_info.framebuffers.size(); i++)
    vkDestroyFramebuffer(logical_device_info.device, framebuffer_info.framebuffers[i], nullptr);
  return 0;
}
