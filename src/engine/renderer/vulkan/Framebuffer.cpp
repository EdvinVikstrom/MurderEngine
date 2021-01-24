#include "Vulkan.hpp"
#include "Util.hpp"

int me::vulkan::Vulkan::setup_framebuffer(const FramebufferInfo &framebuffer_info, Framebuffer &_framebuffer)
{
  for (uint32_t i = 0; i < swapchain_images.size(); i++)
  {
    SwapchainImage &swapchain_image = swapchain_images[i];

    uint32_t attachment_count = 1;
    VkImageView attachments[attachment_count];
    attachments[0] = swapchain_image.image_view;

    VkFramebufferCreateInfo framebuffer_create_info = { };
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.pNext = nullptr;
    framebuffer_create_info.flags = 0;
    framebuffer_create_info.renderPass = vk_render_pass;
    framebuffer_create_info.attachmentCount = attachment_count;
    framebuffer_create_info.pAttachments = attachments;
    framebuffer_create_info.width = framebuffer_info.size[0];
    framebuffer_create_info.height = framebuffer_info.size[1];
    framebuffer_create_info.layers = framebuffer_info.layers;

    VkFramebuffer framebuffer;
    VkResult result = vkCreateFramebuffer(vk_device, &framebuffer_create_info, vk_allocation, &framebuffer);
    if (result != VK_SUCCESS)
      throw exception("failed to create framebuffer [%s]", util::get_result_string(result));

    _framebuffer = framebuffer_infos.size();
    framebuffer_infos.push_back(framebuffer_info);
    swapchain_image.vk_framebuffers.push_back(framebuffer);
  }
  return 0;
}

int me::vulkan::Vulkan::cleanup_framebuffers(const array_proxy<Framebuffer, uint32_t> &_framebuffers)
{
  for (SwapchainImage &swapchain_image : swapchain_images)
  {
    for (Framebuffer framebuffer : _framebuffers)
      vkDestroyFramebuffer(vk_device, swapchain_image.vk_framebuffers[framebuffer], vk_allocation);
  }
  return 0;
}
