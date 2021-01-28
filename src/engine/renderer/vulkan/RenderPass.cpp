#include "Vulkan.hpp"
#include "Util.hpp"

int me::Vulkan::create_render_pass(const RenderPassCreateInfo &render_pass_create_info, RenderPass &render_pass)
{
  VERIFY_CREATE_INFO(render_pass_create_info, STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(render_pass_create_info.device)->vk_device;
  VulkanSwapchain* swapchain = reinterpret_cast<VulkanSwapchain*>(render_pass_create_info.swapchain);
  
  uint32_t subpass_dependency_count = 1;
  VkSubpassDependency subpass_dependencies[subpass_dependency_count];
  subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependencies[0].dstSubpass = 0;
  subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependencies[0].srcAccessMask = 0;
  subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subpass_dependencies[0].dependencyFlags = 0;

  uint32_t color_attachment_description_count = 1;
  VkAttachmentDescription color_attachment_descriptions[color_attachment_description_count];
  color_attachment_descriptions[0].flags = 0;
  color_attachment_descriptions[0].format = swapchain->vk_image_format;
  color_attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment_descriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment_descriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment_descriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  uint32_t color_attachment_reference_count = 1;
  VkAttachmentReference color_attachment_references[color_attachment_reference_count];
  color_attachment_references[0].attachment = 0;
  color_attachment_references[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  uint32_t subpass_description_count = 1;
  VkSubpassDescription vk_subpass_descriptions[subpass_description_count];
  vk_subpass_descriptions[0].flags = 0;
  vk_subpass_descriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  vk_subpass_descriptions[0].inputAttachmentCount = 0;
  vk_subpass_descriptions[0].pInputAttachments = nullptr;
  vk_subpass_descriptions[0].colorAttachmentCount = color_attachment_reference_count;
  vk_subpass_descriptions[0].pColorAttachments = color_attachment_references;
  vk_subpass_descriptions[0].pResolveAttachments = nullptr;
  vk_subpass_descriptions[0].pDepthStencilAttachment = nullptr;
  vk_subpass_descriptions[0].preserveAttachmentCount = 0;
  vk_subpass_descriptions[0].pPreserveAttachments = nullptr;

  VkRenderPassCreateInfo vk_render_pass_create_info = { };
  vk_render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  vk_render_pass_create_info.pNext = nullptr;
  vk_render_pass_create_info.flags = 0;
  vk_render_pass_create_info.attachmentCount = color_attachment_description_count;
  vk_render_pass_create_info.pAttachments = color_attachment_descriptions;
  vk_render_pass_create_info.subpassCount = subpass_description_count;
  vk_render_pass_create_info.pSubpasses = vk_subpass_descriptions;
  vk_render_pass_create_info.dependencyCount = subpass_dependency_count;
  vk_render_pass_create_info.pDependencies = subpass_dependencies;

  VkRenderPass vk_render_pass;
  VkResult result = vkCreateRenderPass(vk_device, &vk_render_pass_create_info, vk_allocation, &vk_render_pass);
  if (result != VK_SUCCESS)
    throw exception("failed to create render pass [%s]", util::get_result_string(result));

  render_pass = alloc.allocate<VulkanRenderPass>(vk_render_pass);
  return 0;
}

int me::Vulkan::cleanup_render_pass(const RenderPassCleanupInfo &render_pass_cleanup_info, RenderPass render_pass_ptr)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(render_pass_cleanup_info.device)->vk_device;
  VulkanRenderPass* render_pass = reinterpret_cast<VulkanRenderPass*>(render_pass_ptr);

  vkDestroyRenderPass(vk_device, render_pass->vk_render_pass, vk_allocation);
  return 0;
}
