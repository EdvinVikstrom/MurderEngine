#include "Vulkan.hpp"
#include "Util.hpp"

int me::Vulkan::create_framebuffer(const FramebufferCreateInfo &framebuffer_info, Framebuffer &framebuffer)
{
  VERIFY_CREATE_INFO(framebuffer_info, STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(framebuffer_info.device)->vk_device;
  VkRenderPass vk_render_pass = reinterpret_cast<RenderPass_T*>(framebuffer_info.render_pass)->vk_render_pass;
  VkImageView vk_image_view = reinterpret_cast<SwapchainImage_T*>(framebuffer_info.image)->vk_image_view;

  uint32_t attachment_count = 1;
  VkImageView attachments[attachment_count];
  attachments[0] = vk_image_view;

  VkFramebufferCreateInfo framebuffer_create_info = { };
  framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebuffer_create_info.pNext = nullptr;
  framebuffer_create_info.flags = 0;
  framebuffer_create_info.renderPass = vk_render_pass;
  framebuffer_create_info.attachmentCount = attachment_count;
  framebuffer_create_info.pAttachments = attachments;
  framebuffer_create_info.width = framebuffer_info.size[0];
  framebuffer_create_info.height = framebuffer_info.size[1];
  framebuffer_create_info.layers = 1; /* TODO: look at this */

  VkFramebuffer vk_framebuffer;
  VkResult result = vkCreateFramebuffer(vk_device, &framebuffer_create_info, vk_allocation, &vk_framebuffer);
  if (result != VK_SUCCESS)
    throw exception("failed to create framebuffer [%s]", util::get_result_string(result));

  framebuffer = alloc.allocate<Framebuffer_T>(vk_framebuffer);
  return 0;
}

int me::Vulkan::cleanup_framebuffer(Device device, Framebuffer framebuffer)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkFramebuffer vk_framebuffer = reinterpret_cast<Framebuffer_T*>(framebuffer)->vk_framebuffer;

  vkDestroyFramebuffer(vk_device, vk_framebuffer, vk_allocation);
  return 0;
}
