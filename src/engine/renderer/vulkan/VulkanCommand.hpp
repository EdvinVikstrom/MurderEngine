#include "Vulkan.inc"

/* <--- SETUP ---> */
int me::Vulkan::setup_command_pool()
{
  logger.debug("> SETUP_COMMAND_POOL");

  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = 0;
  command_pool_create_info.queueFamilyIndex = physical_device_info.queue_family_indices.graphics.value();
 
  VkResult result = vkCreateCommandPool(logical_device_info.device, &command_pool_create_info, nullptr, &command_pool_info.command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));
  return 0;
}

int me::Vulkan::setup_command_buffers()
{
  logger.debug("> SETUP_COMMAND_BUFFERS");

  alloc.allocate_array(framebuffer_info.framebuffers.count, command_buffers);

  VkCommandBufferAllocateInfo command_buffer_allocate_info = { };
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.pNext = nullptr;
  command_buffer_allocate_info.commandPool = command_pool_info.command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = command_buffers.count;

  VkResult result = vkAllocateCommandBuffers(logical_device_info.device, &command_buffer_allocate_info, command_buffers.ptr);
  if (result != VK_SUCCESS)
    throw exception("failed to allocate command buffers [%s]", vk_utils_result_string(result));

  for (uint32_t i = 0; i < command_buffers.count; i++)
  {
    /* create command buffer begin info */
    VkCommandBufferBeginInfo command_buffer_begin_info = { };
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    /* begin command buffer */
    result = vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info);
    if (result != VK_SUCCESS)
      throw exception("failed to begin command buffer [%s]", vk_utils_result_string(result));

    start_render_pass(command_buffers[i], framebuffer_info.framebuffers[i]);

    /* end command buffer */
    result = vkEndCommandBuffer(command_buffers[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to end command buffer [%s]", result);
  }
  return 0;
}

/* <--- RENDERING ---> */
int me::Vulkan::start_render_pass(VkCommandBuffer command_buffer, VkFramebuffer framebuffer)
{
  /* create render pass begin info */
  VkRenderPassBeginInfo render_pass_begin_info = { };
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.pNext = nullptr;
  render_pass_begin_info.renderPass = render_pass_info.render_pass;
  render_pass_begin_info.framebuffer = framebuffer;
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = swapchain_info.image_extent;

  /* create clear values */
  uint32_t clear_value_count = 1;
  VkClearValue clear_values[clear_value_count];
  clear_values[0] = { 0.0F, 0.0F, 0.0F, 1.0F };

  render_pass_begin_info.clearValueCount = clear_value_count;
  render_pass_begin_info.pClearValues = clear_values;

  const size_t vertex_buffer_count = data_storage.meshes.size();
  VkBuffer vertex_buffers[vertex_buffer_count];
  VkDeviceSize offsets[vertex_buffer_count];
  for (size_t i = 0; i < vertex_buffer_count; i++)
  {
    vertex_buffers[i] = data_storage.meshes.at(i)->vertex_buffer;
    offsets[i] = 0;
  }

  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_info.pipeline);
  vkCmdBindVertexBuffers(command_buffer, 0, vertex_buffer_count, vertex_buffers, offsets);

  for (const Mesh* mesh : data_storage.meshes)
    vkCmdDraw(command_buffer, static_cast<uint32_t>(mesh->vertices.size()), 1, 0, 0);

  vkCmdEndRenderPass(command_buffer);
  return 0;
}

/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_command_pool()
{
  vkDestroyCommandPool(logical_device_info.device, command_pool_info.command_pool, nullptr);
  return 0;
}

int me::Vulkan::cleanup_command_buffers()
{
  vkFreeCommandBuffers(logical_device_info.device, command_pool_info.command_pool, command_buffers.count, command_buffers.ptr);
  return 0;
}
