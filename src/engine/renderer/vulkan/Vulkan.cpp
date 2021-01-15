#include "Vulkan.inc"

#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanMemory.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanMesh.hpp"
#include "VulkanCommand.hpp"
#include "VulkanDebug.hpp"

#include "VulkanCallback.hpp"

me::Vulkan::Vulkan(Surface* me_surface)
  : Renderer("vulkan"), me_surface(me_surface), logger("Vulkan")
{
}

int me::Vulkan::initialize(const ModuleInfo module_info)
{
#ifndef NDEBUG
  //logger.trace(Logger::DEBUG, true);
#endif

  alloc = MemoryAlloc(module_info.allocator.child(2048));

  setup_extensions();
  setup_layers();
  setup_instance(module_info.engine_info);
  setup_surface();
#ifndef NDEBUG
  setup_debug_messenger();
  setup_debug_report();
#endif
  setup_device_extensions();
  setup_device_layers();
  setup_physical_device();
  setup_logical_device();
  setup_memory();
  setup_swapchain();
  setup_image_views();
  setup_render_pass();
  setup_shaders();
  setup_pipeline_layout();
  setup_graphics_pipeline();
  setup_framebuffers();
  setup_command_pool();
  setup_vertex_buffers();
  setup_command_buffers();
  setup_synchronization();

  render_info.state = RenderInfo::ACTIVE_STATE;

  return 0;
}

int me::Vulkan::terminate(const ModuleInfo module_info)
{
  vkDeviceWaitIdle(logical_device_info.device);

#ifndef NDEBUG
  cleanup_debug_messenger();
  cleanup_debug_report();
#endif

  cleanup_synchronization();
  cleanup_command_buffers();
  cleanup_vertex_buffers();
  cleanup_command_pool();
  cleanup_framebuffers();
  cleanup_graphics_pipeline();
  cleanup_pipeline_layout();
  cleanup_shaders();
  cleanup_render_pass();
  cleanup_image_views();
  cleanup_swapchain();
  cleanup_surface();
  cleanup_memory();
  cleanup_logical_device();
  cleanup_instance();

  return 0;
}

int me::Vulkan::tick(const ModuleInfo module_info)
{
  if (render_info.state == RenderInfo::ACTIVE_STATE)
    render(render_info);
  else if (render_info.state == RenderInfo::NO_SWAPCHAIN_STATE)
    refresh_swapchains();
  return 0;
}


int me::Vulkan::render(RenderInfo &render_info)
{
  vkWaitForFences(logical_device_info.device, 1, &synchronization_info.in_flight[render_info.frame_index], VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(logical_device_info.device, swapchain_info.swapchain, UINT64_MAX,
      synchronization_info.image_available[render_info.frame_index], VK_NULL_HANDLE, &image_index);

  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    refresh_swapchains();
    return 0;
  }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw exception("failed to acquire next image [%s]", vk_utils_result_string(result));


  if (synchronization_info.images_in_flight[image_index] != VK_NULL_HANDLE)
    vkWaitForFences(logical_device_info.device, 1, &synchronization_info.images_in_flight[image_index], VK_TRUE, UINT64_MAX);
  synchronization_info.images_in_flight[image_index] = synchronization_info.in_flight[render_info.frame_index];


  /* create wait semaphores */
  static uint32_t wait_semaphore_count = 1;
  VkSemaphore wait_semaphores[wait_semaphore_count];
  wait_semaphores[0] = synchronization_info.image_available[render_info.frame_index];

  /* create wait stages */
  static uint32_t wait_stage_count = 1;
  VkPipelineStageFlags wait_stages[wait_stage_count];
  wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  /* get command buffers */
  static uint32_t command_buffer_count = 1;
  VkCommandBuffer command_buffers[command_buffer_count];
  command_buffers[0] = this->command_buffers[image_index];

  /* create signal semaphores */
  static uint32_t signal_semaphore_count = 1;
  VkSemaphore signal_semaphores[signal_semaphore_count];
  signal_semaphores[0] = synchronization_info.render_finished[render_info.frame_index];

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

  vkResetFences(logical_device_info.device, 1, &synchronization_info.in_flight[render_info.frame_index]);

  result = vkQueueSubmit(queue_info.graphics_queue, submit_info_count, submit_infos, synchronization_info.in_flight[render_info.frame_index]);
  if (result != VK_SUCCESS)
    throw exception("failed to queue submit [%s]", vk_utils_result_string(result));


  /* get swapchains */
  static uint32_t swapchain_count = 1;
  VkSwapchainKHR swapchains[swapchain_count];
  swapchains[0] = swapchain_info.swapchain;

  /* create present info */
  VkPresentInfoKHR present_info = { };
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.waitSemaphoreCount = signal_semaphore_count;
  present_info.pWaitSemaphores = signal_semaphores;
  present_info.swapchainCount = swapchain_count;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &image_index;
  present_info.pResults = nullptr;

  result = vkQueuePresentKHR(queue_info.present_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      render_info.flags & RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT)
  {
    render_info.flags &= ~RenderInfo::FRAMEBUFFER_RESIZED_FLAG_BIT;
    refresh_swapchains();
  }else if (result != VK_SUCCESS)
    throw exception("failed to queue present [%s]", vk_utils_result_string(result));


  /* increment the frame index and reset to 0 if equals MAX_FRAMES_IN_FLIGHT */
  render_info.frame_index++;
  if (render_info.frame_index == MAX_FRAMES_IN_FLIGHT)
    render_info.frame_index = 0;
  return 0;
}


int me::Vulkan::register_shader(Shader* shader)
{
  shader_info.shaders.push_back(shader);
  return 0;
}

int me::Vulkan::register_mesh(Mesh* mesh)
{
  data_storage.meshes.push_back(mesh);
  return 0;
}
