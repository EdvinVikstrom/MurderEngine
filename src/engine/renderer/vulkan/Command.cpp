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
    uint32_t buffer_count, CommandBuffer* buffers)
{
  VERIFY_CREATE_INFO(command_buffer_create_info, STRUCTURE_TYPE_COMMAND_BUFFER_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(command_buffer_create_info.device)->vk_device;
  VkCommandPool vk_command_pool = reinterpret_cast<CommandPool_T*>(command_buffer_create_info.command_pool)->vk_command_pool;

  VkCommandBuffer vk_command_buffers[buffer_count];
  allocate_command_buffers(vk_device, vk_allocation, vk_command_pool, buffer_count, vk_command_buffers);

  for (uint32_t i = 0; i < buffer_count; i++)
    buffers[i] = alloc.allocate<CommandBuffer_T>(vk_command_buffers[i], command_buffer_create_info.usage);
  return 0;
}

int me::Vulkan::cmd_record_start(CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;

  VkCommandBufferBeginInfo command_buffer_begin_info = { };
  command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  command_buffer_begin_info.pNext = nullptr;
  command_buffer_begin_info.flags = 0;
  command_buffer_begin_info.pInheritanceInfo = nullptr;

  VkResult result = vkBeginCommandBuffer(vk_command_buffer, &command_buffer_begin_info);
  if (result != VK_SUCCESS)
    throw exception("failed to begin command buffer [%s]", util::get_result_string(result));
  return 0;
}

int me::Vulkan::cmd_record_stop(CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;

  VkResult result = vkEndCommandBuffer(vk_command_buffer);
  if (result != VK_SUCCESS)
    throw exception("failed to end command buffer [%s]", result);
  return 0;
}

int me::Vulkan::cmd_begin_render_pass(const CmdBeginRenderPassInfo &cmd_begin_render_pass_info, CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;
  VkRenderPass vk_render_pass = reinterpret_cast<RenderPass_T*>(cmd_begin_render_pass_info.render_pass)->vk_render_pass;
  VkFramebuffer vk_framebuffer = reinterpret_cast<Framebuffer_T*>(cmd_begin_render_pass_info.framebuffer)->vk_framebuffer;
  VkExtent2D vk_image_extent = reinterpret_cast<Swapchain_T*>(cmd_begin_render_pass_info.swapchain)->vk_image_extent;

  CommandBufferUsage command_buffer_usage = reinterpret_cast<CommandBuffer_T*>(command_buffer)->usage;

  if (command_buffer_usage != COMMAND_BUFFER_USAGE_RENDERING)
    throw exception("in 'cmd_begin_render_pass()' 'CommandBuffer[%p]::usage' must be 'COMMAND_BUFFER_USAGE_RENDERING'", command_buffer);

  /* create render pass begin info */
  VkRenderPassBeginInfo render_pass_begin_info = { };
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.pNext = nullptr;
  render_pass_begin_info.renderPass = vk_render_pass;
  render_pass_begin_info.framebuffer = vk_framebuffer;
  render_pass_begin_info.renderArea.offset = {0, 0};
  render_pass_begin_info.renderArea.extent = vk_image_extent;

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
  vkCmdBeginRenderPass(vk_command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
  return 0;
}

int me::Vulkan::cmd_end_render_pass(CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;

  vkCmdEndRenderPass(vk_command_buffer);
  return 0;
}

int me::Vulkan::cmd_bind_descriptors(const CmdBindDescriptorsInfo &cmd_bind_descriptors_info, CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;
  VkPipelineLayout vk_pipeline_layout = reinterpret_cast<Pipeline_T*>(cmd_bind_descriptors_info.pipeline)->vk_layout;

  VkDescriptorSet vk_descriptor_sets[cmd_bind_descriptors_info.descriptor_count];
  for (uint32_t i = 0; i < cmd_bind_descriptors_info.descriptor_count; i++)
    vk_descriptor_sets[i] = reinterpret_cast<Descriptor_T*>(cmd_bind_descriptors_info.descriptors[i])->vk_descriptor_set;

  vkCmdBindDescriptorSets(vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
      vk_pipeline_layout, 0, cmd_bind_descriptors_info.descriptor_count, vk_descriptor_sets, 0, nullptr);
  return 0;
}

int me::Vulkan::cmd_draw_meshes(const CmdDrawMeshesInfo &cmd_draw_meshes_info, CommandBuffer command_buffer)
{
  VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(command_buffer)->vk_command_buffer;
  VkPipeline vk_pipeline = reinterpret_cast<Pipeline_T*>(cmd_draw_meshes_info.pipeline)->vk_pipeline;

  for (uint32_t i = 0; i < cmd_draw_meshes_info.mesh_count; i++)
  {
    Mesh* mesh = cmd_draw_meshes_info.meshes[i];
    VkBuffer vk_vertex_buffer = reinterpret_cast<Buffer_T*>(mesh->vertex_buffer)->vk_buffer;
    VkBuffer vk_index_buffer = reinterpret_cast<Buffer_T*>(mesh->index_buffer)->vk_buffer;

    /* get vertex buffers and offsets */
    const size_t vertex_buffer_count = 1;
    VkBuffer vk_vertex_buffers[vertex_buffer_count] = {vk_vertex_buffer};
    VkDeviceSize vk_offsets[vertex_buffer_count] = {0};

    vkCmdBindPipeline(vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);
    vkCmdBindVertexBuffers(vk_command_buffer, 0, vertex_buffer_count, vk_vertex_buffers, vk_offsets);
    vkCmdBindIndexBuffer(vk_command_buffer, vk_index_buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(vk_command_buffer, static_cast<uint32_t>(mesh->indices.size()), 1, 0, 0, 0);
  }
  return 0;
}


int me::Vulkan::cleanup_command_buffers(Device device, CommandPool command_pool, uint32_t buffer_count, CommandBuffer* buffers)
{
  VkDevice vk_device = reinterpret_cast<Device_T*>(device)->vk_device;
  VkCommandPool vk_command_pool = reinterpret_cast<CommandPool_T*>(command_pool)->vk_command_pool;

  VkCommandBuffer vk_command_buffers[buffer_count];
  for (uint32_t i = 0; i < buffer_count; i++)
  {
    VkCommandBuffer vk_command_buffer = reinterpret_cast<CommandBuffer_T*>(buffers[i])->vk_command_buffer;
    vk_command_buffers[i] = vk_command_buffer;
  }

  vkFreeCommandBuffers(vk_device, vk_command_pool, buffer_count, vk_command_buffers);
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
