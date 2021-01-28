#include "Vulkan.hpp"
#include "Util.hpp"

int me::Vulkan::create_frames(const FrameCreateInfo &frame_create_info, uint32_t frame_count, Frame* frames)
{
  VERIFY_CREATE_INFO(frame_create_info, STRUCTURE_TYPE_FRAME_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(frame_create_info.device)->vk_device;

  VkSemaphoreCreateInfo semaphore_create_info = { };
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = { };
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < frame_count; i++)
  {
    /* create 'image available' semaphore */
    VkSemaphore vk_image_available_semaphore;
    VkResult result = vkCreateSemaphore(vk_device, &semaphore_create_info, vk_allocation, &vk_image_available_semaphore);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", util::get_result_string(result));

    /* create 'render finished' semaphore */
    VkSemaphore vk_render_finished_semaphore;
    result = vkCreateSemaphore(vk_device, &semaphore_create_info, vk_allocation, &vk_render_finished_semaphore);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", util::get_result_string(result));

    /* create 'in flight' fence */
    VkFence vk_in_flight_fence;
    result = vkCreateFence(vk_device, &fence_create_info, vk_allocation, &vk_in_flight_fence);
    if (result != VK_SUCCESS)
      throw exception("failed to create fence [%s]", util::get_result_string(result));

    frames[i] = alloc.allocate<VulkanFrame>(vk_image_available_semaphore, vk_render_finished_semaphore, vk_in_flight_fence);
  }
  return 0;
}

int me::Vulkan::cleanup_frames(const FrameCleanupInfo &frame_cleanup_info, uint32_t frame_count, Frame* frames)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(frame_cleanup_info.device)->vk_device;

  for (uint32_t i = 0; i < RenderInfo::MAX_FRAMES_IN_FLIGHT; i++)
  {
    VulkanFrame* frame = reinterpret_cast<VulkanFrame*>(frames[i]);

    vkDestroySemaphore(vk_device, frame->vk_image_available_semaphore, vk_allocation);
    vkDestroySemaphore(vk_device, frame->vk_render_finished_semaphore, vk_allocation);
    vkDestroyFence(vk_device, frame->vk_in_flight_fence, vk_allocation);
  }
  return 0;
}
