#include "Vulkan.hpp"
#include "Util.hpp"

me::vulkan::Vulkan::Vulkan()
  : Renderer("vulkan"), logger("Vulkan")
{
}

int me::vulkan::Vulkan::init_engine(const EngineInfo &engine_info, Surface* surface)
{
  setup_instance(engine_info, surface);
  setup_debug();

  render_info.state = RenderInfo::ACTIVE_STATE;
  return 0;
}

int me::vulkan::Vulkan::initialize(const ModuleInfo module_info)
{
#ifndef NDEBUG
  //logger.trace(Logger::DEBUG, true);
#endif

  alloc = MemoryAlloc(module_info.alloc.child(2048));
  return 0;
}

int me::vulkan::Vulkan::terminate(const ModuleInfo module_info)
{
  vkDeviceWaitIdle(vk_device);

#ifndef NDEBUG
  cleanup_debug();
#endif

  cleanup_memory();
  cleanup_command_pool();
  cleanup_render_pass();
  cleanup_swapchain();
  cleanup_surface();
  cleanup_memory();
  cleanup_device();
  cleanup_instance();
  return 0;
}

int me::vulkan::Vulkan::tick(const ModuleInfo module_info)
{
  if (render_info.state == RenderInfo::ACTIVE_STATE)
    render(render_info);
  else if (render_info.state == RenderInfo::NO_SWAPCHAIN_STATE)
    refresh_swapchain();
  return 0;
}


int me::vulkan::Vulkan::render(RenderInfo &render_info)
{
  vkWaitForFences(vk_device, 1, &vk_in_flight_fences[render_info.frame_index], VK_TRUE, UINT64_MAX);

  VkResult result = vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX,
      vk_image_available_semaphores[render_info.frame_index], VK_NULL_HANDLE, &render_info.image_index);

  SwapchainImage &swapchain_image = swapchain_images[render_info.image_index];

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    refresh_swapchain();
    return 0;
  }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw exception("failed to acquire next image [%s]", util::get_result_string(result));


  if (vk_images_in_flight_fences[render_info.image_index] != VK_NULL_HANDLE)
    vkWaitForFences(vk_device, 1, &vk_images_in_flight_fences[render_info.image_index], VK_TRUE, UINT64_MAX);
  vk_images_in_flight_fences[render_info.image_index] = vk_in_flight_fences[render_info.frame_index];

  refresh_uniform_buffers(render_info);

  /* create wait semaphores */
  static uint32_t wait_semaphore_count = 1;
  VkSemaphore wait_semaphores[wait_semaphore_count];
  wait_semaphores[0] = vk_image_available_semaphores[render_info.frame_index];

  /* create wait stages */
  static uint32_t wait_stage_count = 1;
  VkPipelineStageFlags wait_stages[wait_stage_count];
  wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  /* get command buffers */
  static uint32_t command_buffer_count = swapchain_image.vk_command_buffers.size();
  VkCommandBuffer command_buffers[command_buffer_count];
  for (uint32_t i = 0; i < swapchain_image.vk_command_buffers.size(); i++)
    command_buffers[i] = swapchain_image.vk_command_buffers[i];

  /* create signal semaphores */
  static uint32_t signal_semaphore_count = 1;
  VkSemaphore signal_semaphores[signal_semaphore_count];
  signal_semaphores[0] = vk_render_finished_semaphores[render_info.frame_index];

  /* create submit infos */
  static uint32_t submit_info_count = 1;
  VkSubmitInfo submit_infos[submit_info_count];
  submit_infos[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_infos[0].pNext = nullptr;
  submit_infos[0].waitSemaphoreCount = wait_semaphore_count;
  submit_infos[0].pWaitSemaphores = wait_semaphores;
  submit_infos[0].pWaitDstStageMask = wait_stages;
  submit_infos[0].commandBufferCount = command_buffer_count;
  submit_infos[0].pCommandBuffers = command_buffers;
  submit_infos[0].signalSemaphoreCount = signal_semaphore_count;
  submit_infos[0].pSignalSemaphores = signal_semaphores;

  vkResetFences(vk_device, 1, &vk_in_flight_fences[render_info.frame_index]);

  result = vkQueueSubmit(queue_families.graphics_queue, submit_info_count, submit_infos, vk_in_flight_fences[render_info.frame_index]);
  if (result != VK_SUCCESS)
    throw exception("failed to queue submit [%s]", util::get_result_string(result));


  /* get swapchains */
  static uint32_t swapchain_count = 1;
  VkSwapchainKHR swapchains[swapchain_count];
  swapchains[0] = vk_swapchain;

  /* create present info */
  VkPresentInfoKHR present_info = { };
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.waitSemaphoreCount = signal_semaphore_count;
  present_info.pWaitSemaphores = signal_semaphores;
  present_info.swapchainCount = swapchain_count;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &render_info.image_index;
  present_info.pResults = nullptr;

  result = vkQueuePresentKHR(queue_families.present_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      render_info.flags & RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT)
  {
    render_info.flags &= ~RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
    refresh_swapchain();
  }else if (result != VK_SUCCESS)
    throw exception("failed to queue present [%s]", util::get_result_string(result));


  /* increment the frame index and reset to 0 if equals MAX_FRAMES_IN_FLIGHT */
  render_info.frame_index++;
  if (render_info.frame_index == RenderInfo::MAX_FRAMES_IN_FLIGHT)
    render_info.frame_index = 0;
  return 0;
}

int me::vulkan::Vulkan::register_mesh(MeshRef* mesh)
{
  data_storage.meshes.push_back(mesh);
  return 0;
}
