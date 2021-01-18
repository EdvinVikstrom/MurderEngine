#include "Vulkan.inc"
#include <vulkan/vulkan_core.h>

/* <--- SETUP ---> */
int me::Vulkan::setup_command_pool()
{
  logger.debug("> SETUP_COMMAND_POOL");

  create_command_pool(physical_device_info.queue_family_indices.graphics.value(), command_pool_info.graphics_command_pool);
  create_command_pool(physical_device_info.queue_family_indices.transfer.value(), command_pool_info.transfer_command_pool);
  return 0;
}

int me::Vulkan::setup_command_buffers()
{
  logger.debug("> SETUP_COMMAND_BUFFERS");

  uint32_t draw_command_buffer_count = framebuffer_info.framebuffers.size();
  command_buffer_info.draw_command_buffers.resize(draw_command_buffer_count);

  VkCommandBufferAllocateInfo command_buffer_allocate_info = { };
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.pNext = nullptr;
  command_buffer_allocate_info.commandPool = command_pool_info.graphics_command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = draw_command_buffer_count;

  VkResult result = vkAllocateCommandBuffers(logical_device_info.device, &command_buffer_allocate_info, command_buffer_info.draw_command_buffers.data());
  if (result != VK_SUCCESS)
    throw exception("failed to allocate command buffers [%s]", vk_utils_result_string(result));

  /* 'drawing' command buffers */
  for (uint32_t i = 0; i < draw_command_buffer_count; i++)
  {
    /* create command buffer begin info */
    VkCommandBufferBeginInfo command_buffer_begin_info = { };
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    /* begin command buffer */
    result = vkBeginCommandBuffer(command_buffer_info.draw_command_buffers[i], &command_buffer_begin_info);
    if (result != VK_SUCCESS)
      throw exception("failed to begin command buffer [%s]", vk_utils_result_string(result));

    start_render_pass(command_buffer_info.draw_command_buffers[i], framebuffer_info.framebuffers[i]);

    /* end command buffer */
    result = vkEndCommandBuffer(command_buffer_info.draw_command_buffers[i]);
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
  clear_values[0] = {0.0F, 0.0F, 0.0F, 1.0F};

  render_pass_begin_info.clearValueCount = clear_value_count;
  render_pass_begin_info.pClearValues = clear_values;

  /* begin recording */
  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

  /* draw every visible mesh */
  for (const Mesh* mesh : data_storage.meshes)
  {
    /* get vertex buffers and offsets */
    const size_t vertex_buffer_count = 1;
    VkBuffer vertex_buffers[vertex_buffer_count] = {mesh->vertex_buffer};
    VkDeviceSize offsets[vertex_buffer_count] = {1};

    /* get index buffer */
    VkBuffer index_buffer = mesh->index_buffer;


    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_info.pipeline);
    vkCmdBindVertexBuffers(command_buffer, 0, vertex_buffer_count, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
  }

  /* stop recording */
  vkCmdEndRenderPass(command_buffer);
  return 0;
}


/* <--- HELPERS ---> */
int me::Vulkan::create_command_pool(const uint32_t queue_family_index, VkCommandPool &command_pool)
{
  VkCommandPoolCreateInfo command_pool_create_info = { };
  command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  command_pool_create_info.pNext = nullptr;
  command_pool_create_info.flags = 0;
  command_pool_create_info.queueFamilyIndex = queue_family_index;
 
  VkResult result = vkCreateCommandPool(logical_device_info.device, &command_pool_create_info, nullptr, &command_pool);
  if (result != VK_SUCCESS)
    throw exception("failed to create command pool [%s]", vk_utils_result_string(result));
  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_command_pool()
{
  vkDestroyCommandPool(logical_device_info.device, command_pool_info.graphics_command_pool, nullptr);
  vkDestroyCommandPool(logical_device_info.device, command_pool_info.transfer_command_pool, nullptr);
  return 0;
}

int me::Vulkan::cleanup_command_buffers()
{
  vkFreeCommandBuffers(logical_device_info.device, command_pool_info.graphics_command_pool,
      command_buffer_info.draw_command_buffers.size(), command_buffer_info.draw_command_buffers.data());
  return 0;
}
