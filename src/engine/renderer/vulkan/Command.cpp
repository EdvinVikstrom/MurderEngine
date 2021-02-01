#include "Vulkan.hpp"
#include "Util.hpp"

static int allocate_command_buffers(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkCommandPool 				command_pool,
    uint32_t 					count,
    VkCommandBuffer*				command_buffers
    );


int me::Vulkan::create_command_buffers(const CommandBufferCreateInfo &command_buffer_create_info,
    uint32_t command_buffer_count, CommandBuffer* command_buffers)
{
  VERIFY_CREATE_INFO(command_buffer_create_info, STRUCTURE_TYPE_COMMAND_BUFFER_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(command_buffer_create_info.device)->vk_device;
  VulkanCommandPool* command_pool = reinterpret_cast<VulkanCommandPool*>(command_buffer_create_info.command_pool);

  VkCommandBuffer vk_command_buffers[command_buffer_count];
  allocate_command_buffers(vk_device, vk_allocation, command_pool->vk_command_pool, command_buffer_count, vk_command_buffers);

  for (uint32_t i = 0; i < command_buffer_count; i++)
    command_buffers[i] = alloc.allocate<VulkanCommandBuffer>(vk_command_buffers[i], command_buffer_create_info.usage);
  return 0;
}

int me::Vulkan::cmd_record_start(const CommandInfo &command_info, CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);

  VkCommandBufferBeginInfo command_buffer_begin_info = { };
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.pNext = nullptr;
  command_buffer_begin_info.flags = 0;
  command_buffer_begin_info.pInheritanceInfo = nullptr;

  VkResult result = vkBeginCommandBuffer(vulkan_command_buffer->vk_command_buffer, &command_buffer_begin_info);
  if (result != VK_SUCCESS)
    throw exception("failed to begin command buffer [%s]", util::get_result_string(result));
  return 0;
}

int me::Vulkan::cmd_record_stop(const CommandInfo &command_info, CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);

  VkResult result = vkEndCommandBuffer(vulkan_command_buffer->vk_command_buffer);
  if (result != VK_SUCCESS)
    throw exception("failed to end command buffer [%s]", result);
  return 0;
}

int me::Vulkan::cmd_begin_render_pass(const CmdBeginRenderPassInfo &cmd_begin_render_pass_info, CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);
  VulkanRenderPass* render_pass = reinterpret_cast<VulkanRenderPass*>(cmd_begin_render_pass_info.render_pass);
  VulkanFramebuffer* framebuffer = reinterpret_cast<VulkanFramebuffer*>(cmd_begin_render_pass_info.framebuffer);
  VulkanSwapchain* swapchain = reinterpret_cast<VulkanSwapchain*>(cmd_begin_render_pass_info.swapchain);

  if (vulkan_command_buffer->usage != COMMAND_BUFFER_USAGE_RENDERING)
    throw exception("in 'cmd_begin_render_pass()' 'CommandBuffer[%p]::usage' must be 'COMMAND_BUFFER_USAGE_RENDERING'", command_buffer);

  /* create render pass begin info */
  VkRenderPassBeginInfo render_pass_begin_info = { };
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.pNext = nullptr;
  render_pass_begin_info.renderPass = render_pass->vk_render_pass;
  render_pass_begin_info.framebuffer = framebuffer->vk_framebuffer;
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = swapchain->vk_image_extent;

  /* create clear values */
  uint32_t clear_value_count = 1;
  VkClearValue clear_values[clear_value_count];
  clear_values[0].color.float32[0] = cmd_begin_render_pass_info.clear_values[0];
  clear_values[0].color.float32[1] = cmd_begin_render_pass_info.clear_values[1];
  clear_values[0].color.float32[2] = cmd_begin_render_pass_info.clear_values[2];
  clear_values[0].color.float32[3] = cmd_begin_render_pass_info.clear_values[3];

  render_pass_begin_info.clearValueCount = clear_value_count;
  render_pass_begin_info.pClearValues = clear_values;

  /* begin recording */
  vkCmdBeginRenderPass(vulkan_command_buffer->vk_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  return 0;
}

int me::Vulkan::cmd_end_render_pass(CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);

  vkCmdEndRenderPass(vulkan_command_buffer->vk_command_buffer);
  return 0;
}

int me::Vulkan::cmd_bind_descriptors(const CmdBindDescriptorsInfo &cmd_bind_descriptors_info, CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);
  VulkanPipeline* pipeline = reinterpret_cast<VulkanPipeline*>(cmd_bind_descriptors_info.pipeline);

  VkDescriptorSet vk_descriptor_sets[cmd_bind_descriptors_info.descriptor_count];
  for (uint32_t i = 0; i < cmd_bind_descriptors_info.descriptor_count; i++)
    vk_descriptor_sets[i] = reinterpret_cast<VulkanDescriptor*>(cmd_bind_descriptors_info.descriptors[i])->vk_descriptor_set;

  vkCmdBindDescriptorSets(vulkan_command_buffer->vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      pipeline->vk_layout, 0, cmd_bind_descriptors_info.descriptor_count, vk_descriptor_sets, 0, nullptr);
  return 0;
}

int me::Vulkan::cmd_draw_meshes(const CmdDrawMeshesInfo &cmd_draw_meshes_info, CommandBuffer command_buffer)
{
  VulkanCommandBuffer* vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffer);
  VulkanPipeline* pipeline = reinterpret_cast<VulkanPipeline*>(cmd_draw_meshes_info.pipeline);

  for (uint32_t i = 0; i < cmd_draw_meshes_info.mesh_count; i++)
  {
    Mesh* mesh = cmd_draw_meshes_info.meshes[i];
    VulkanBuffer* vertex_buffer = reinterpret_cast<VulkanBuffer*>(mesh->vertex_buffer);
    VulkanBuffer* index_buffer = reinterpret_cast<VulkanBuffer*>(mesh->index_buffer);

    /* get vertex buffers and offsets */
    const size_t vertex_buffer_count = 1;
    VkBuffer vk_vertex_buffers[vertex_buffer_count] = {vertex_buffer->vk_buffer};
    VkDeviceSize vk_offsets[vertex_buffer_count] = {0};

    /* get index buffer */
    VkBuffer vk_index_buffer = index_buffer->vk_buffer;

    vkCmdBindPipeline(vulkan_command_buffer->vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->vk_pipeline);
    vkCmdBindVertexBuffers(vulkan_command_buffer->vk_command_buffer, 0, vertex_buffer_count, vk_vertex_buffers, vk_offsets);
    vkCmdBindIndexBuffer(vulkan_command_buffer->vk_command_buffer, vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(vulkan_command_buffer->vk_command_buffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
  }
  return 0;
}


int me::Vulkan::cleanup_command_buffers(const CommandBufferCleanupInfo &command_buffer_cleanup_info,
    uint32_t command_buffer_count, CommandBuffer* command_buffers)
{
  VkDevice vk_device = reinterpret_cast<VulkanDevice*>(command_buffer_cleanup_info.device)->vk_device;
  VulkanCommandPool* command_pool = reinterpret_cast<VulkanCommandPool*>(command_buffer_cleanup_info.command_pool);

  VkCommandBuffer vk_command_buffers[command_buffer_count];
  for (uint32_t i = 0; i < command_buffer_count; i++)
  {
    VulkanCommandBuffer* command_buffer = reinterpret_cast<VulkanCommandBuffer*>(command_buffers[i]);
    vk_command_buffers[i] = command_buffer->vk_command_buffer;
  }

  vkFreeCommandBuffers(vk_device, command_pool->vk_command_pool, command_buffer_count, vk_command_buffers);
  return 0;
}


int allocate_command_buffers(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkCommandPool 				command_pool,
    uint32_t 					count,
    VkCommandBuffer*				command_buffers
    )
{
  VkCommandBufferAllocateInfo command_buffer_allocate_info = { };
  command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_allocate_info.pNext = nullptr;
  command_buffer_allocate_info.commandPool = command_pool;
  command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  command_buffer_allocate_info.commandBufferCount = count;

  VkResult result = vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffers);
  if (result != VK_SUCCESS)
    throw me::exception("failed to allocate command buffers [%s]", me::util::get_result_string(result));
  return 0;
}
