#include "Vulkan.hpp"
#include "Util.hpp"

me::Vulkan::Vulkan()
  : RendererModule("vulkan"), logger("Vulkan")
{
}

int me::Vulkan::init_engine(const EngineInitInfo &engine_init_info)
{
  alloc = engine_init_info.alloc;

  if (engine_init_info.debug)
  {
    debugging = true;
    logger.set_option(LOG_DEBUG_FLAG, true);
  }

  setup_instance(engine_init_info);

  if (debugging)
    setup_debug();
  return 0;
}

int me::Vulkan::terminate_engine()
{
  if (debugging)
    cleanup_debug();
  cleanup_instance();
  return 0;
}

int me::Vulkan::initialize(const ModuleInfo module_info)
{
#ifndef NDEBUG
#endif
  return 0;
}

int me::Vulkan::terminate(const ModuleInfo module_info)
{
  return 0;
}

int me::Vulkan::tick(const ModuleInfo module_info)
{
  return 0;
}


//render_info.frame_index++;
//if (render_info.frame_index == RenderInfo::MAX_FRAMES_IN_FLIGHT)
//  render_info.frame_index = 0;

int me::Vulkan::frame_prepare(const FramePrepareInfo &frame_prepare_info, FramePrepared &frame_prepared)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(frame_prepare_info.device)->vk_device;
  VkSwapchainKHR vk_swapchain = reinterpret_cast<VulkanSwapchain*>(frame_prepare_info.swapchain)->vk_swapchain;
  VulkanFrame* frame = reinterpret_cast<VulkanFrame*>(frame_prepare_info.frame);

  vkWaitForFences(vk_device, 1, &frame->vk_in_flight_fence, VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX,
      frame->vk_image_available_semaphore, VK_NULL_HANDLE, &image_index);

  uint8_t flags = 0;
  if (result == VK_ERROR_OUT_OF_DATE_KHR)
  {
    flags |= FRAME_SWAPCHAIN_REFRESH_FLAG;
  }else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    throw exception("failed to acquire next image [%s]", util::get_result_string(result));

  frame_prepared = flags | (image_index << 8);
  return 0;
}

int me::Vulkan::frame_render(const FrameRenderInfo &frame_render_info, FrameRendered &frame_rendered)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(frame_render_info.device)->vk_device;
  VkQueue vk_queue = reinterpret_cast<VulkanQueue*>(frame_render_info.queue)->vk_queue;
  VulkanSwapchainImage* image = reinterpret_cast<VulkanSwapchainImage*>(frame_render_info.image);
  VulkanFrame* frame = reinterpret_cast<VulkanFrame*>(frame_render_info.frame);

  if (image->vk_in_flight_fence != VK_NULL_HANDLE)
    vkWaitForFences(vk_device, 1, &image->vk_in_flight_fence, VK_TRUE, UINT64_MAX);
  image->vk_in_flight_fence = frame->vk_in_flight_fence;

  // refresh_uniform_buffers(render_info);

  /* create wait semaphores */
  static uint32_t wait_semaphore_count = 1;
  VkSemaphore wait_semaphores[wait_semaphore_count];
  wait_semaphores[0] = frame->vk_image_available_semaphore;

  /* create wait stages */
  static uint32_t wait_stage_count = 1;
  VkPipelineStageFlags wait_stages[wait_stage_count];
  wait_stages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  /* get command buffers */
  VkCommandBuffer command_buffers[frame_render_info.command_buffer_count];
  for (uint32_t i = 0; i < frame_render_info.command_buffer_count; i++)
    command_buffers[i] = reinterpret_cast<VulkanCommandBuffer*>(frame_render_info.command_buffers[i])->vk_command_buffer;

  /* get signal semaphores */
  static constexpr uint32_t signal_semaphore_count = 1;
  VkSemaphore signal_semaphores[signal_semaphore_count];
  signal_semaphores[0] = frame->vk_render_finished_semaphore;

  /* create submit infos */
  static uint32_t submit_info_count = 1;
  VkSubmitInfo submit_infos[submit_info_count];
  submit_infos[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_infos[0].pNext = nullptr;
  submit_infos[0].waitSemaphoreCount = wait_semaphore_count;
  submit_infos[0].pWaitSemaphores = wait_semaphores;
  submit_infos[0].pWaitDstStageMask = wait_stages;
  submit_infos[0].commandBufferCount = frame_render_info.command_buffer_count;
  submit_infos[0].pCommandBuffers = command_buffers;
  submit_infos[0].signalSemaphoreCount = signal_semaphore_count;
  submit_infos[0].pSignalSemaphores = signal_semaphores;

  vkResetFences(vk_device, 1, &image->vk_in_flight_fence);

  VkResult result = vkQueueSubmit(vk_queue, submit_info_count, submit_infos, image->vk_in_flight_fence);
  if (result != VK_SUCCESS)
    throw exception("failed to queue submit [%s]", util::get_result_string(result));

  uint8_t flags = 0;
  frame_rendered = flags;
  return 0;
}

int me::Vulkan::frame_present(const FramePresentInfo &frame_present_info, FramePresented &frame_presented)
{
  VkQueue vk_queue = reinterpret_cast<VulkanQueue*>(frame_present_info.queue)->vk_queue;
  VulkanFrame* frame = reinterpret_cast<VulkanFrame*>(frame_present_info.frame);

  VkSwapchainKHR swapchains[frame_present_info.swapchain_count];
  for (uint32_t i = 0; i < frame_present_info.swapchain_count; i++)
    swapchains[i] = reinterpret_cast<VulkanSwapchain*>(frame_present_info.swapchains[i])->vk_swapchain;

  /* get signal semaphores */
  static constexpr uint32_t signal_semaphore_count = 1;
  VkSemaphore signal_semaphores[signal_semaphore_count];
  signal_semaphores[0] = frame->vk_render_finished_semaphore;

  /* create present info */
  VkPresentInfoKHR present_info = { };
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.pNext = nullptr;
  present_info.waitSemaphoreCount = signal_semaphore_count;
  present_info.pWaitSemaphores = signal_semaphores;
  present_info.swapchainCount = frame_present_info.swapchain_count;
  present_info.pSwapchains = swapchains;
  present_info.pImageIndices = &frame_present_info.image_index;
  present_info.pResults = nullptr;

  uint8_t flags = 0;
  VkResult result = vkQueuePresentKHR(vk_queue, &present_info);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || flags & FRAME_SWAPCHAIN_REFRESH_FLAG)
  {
    flags &= ~FRAME_SWAPCHAIN_REFRESH_FLAG;
  }else if (result != VK_SUCCESS)
    throw exception("failed to queue present [%s]", util::get_result_string(result));

  frame_presented = flags;
  return 0;
}


int me::Vulkan::frame_prepared_get_image_index(FramePrepared frame_prepared, uint32_t &image_index)
{
  image_index = (frame_prepared >> 8) & 0xFF;
  return 0;
}

int me::Vulkan::setup_mesh(const SetupMeshInfo &setup_mesh_info, Mesh* mesh)
{
  size_t vertex_buffer_size = mesh->vertices.size() * sizeof(Vertex);
  size_t index_buffer_size = mesh->indices.size() * sizeof(Index);

  BufferCreateInfo vertex_buffer_create_info = {};
  vertex_buffer_create_info.type = STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertex_buffer_create_info.next = nullptr;
  vertex_buffer_create_info.physical_device = setup_mesh_info.physical_device;
  vertex_buffer_create_info.device = setup_mesh_info.device;
  vertex_buffer_create_info.usage = BUFFER_USAGE_VERTEX_BUFFER;
  vertex_buffer_create_info.write_method = BUFFER_WRITE_METHOD_STAGING;
  vertex_buffer_create_info.size = vertex_buffer_size;
  create_buffer(vertex_buffer_create_info, mesh->vertex_buffer);

  BufferCreateInfo index_buffer_create_info = {};
  index_buffer_create_info.type = STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  index_buffer_create_info.next = nullptr;
  index_buffer_create_info.physical_device = setup_mesh_info.physical_device;
  index_buffer_create_info.device = setup_mesh_info.device;
  index_buffer_create_info.usage = BUFFER_USAGE_INDEX_BUFFER;
  index_buffer_create_info.write_method = BUFFER_WRITE_METHOD_STAGING;
  index_buffer_create_info.size = index_buffer_size;
  create_buffer(index_buffer_create_info, mesh->index_buffer);

  BufferWriteInfo vertex_buffer_wrtie_info = {};
  vertex_buffer_wrtie_info.physical_device = setup_mesh_info.physical_device;
  vertex_buffer_wrtie_info.device = setup_mesh_info.device;
  vertex_buffer_wrtie_info.queue = setup_mesh_info.transfer_queue;
  vertex_buffer_wrtie_info.command_pool = setup_mesh_info.transfer_pool;
  vertex_buffer_wrtie_info.byte_count = vertex_buffer_size;
  vertex_buffer_wrtie_info.bytes = mesh->vertices.data();
  buffer_write(vertex_buffer_wrtie_info, mesh->vertex_buffer);

  BufferWriteInfo index_buffer_wrtie_info = {};
  index_buffer_wrtie_info.physical_device = setup_mesh_info.physical_device;
  index_buffer_wrtie_info.device = setup_mesh_info.device;
  index_buffer_wrtie_info.queue = setup_mesh_info.transfer_queue;
  index_buffer_wrtie_info.command_pool = setup_mesh_info.transfer_pool;
  index_buffer_wrtie_info.byte_count = index_buffer_size;
  index_buffer_wrtie_info.bytes = mesh->indices.data();
  buffer_write(index_buffer_wrtie_info, mesh->index_buffer);
  return 0;
}
