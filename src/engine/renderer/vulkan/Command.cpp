#include "Vulkan.hpp"
#include "Util.hpp"

static int allocate_command_buffers(
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkCommandPool 				command_pool,
    uint32_t 					count,
    VkCommandBuffer*				command_buffers
    );


int me::vulkan::Vulkan::setup_command_buffer(const CommandBufferInfo &command_buffer_info, CommandBuffer &_command_buffer)
{
  VkCommandBuffer command_buffers[swapchain_images.size()];
  allocate_command_buffers(vk_device, vk_allocation, vk_graphics_command_pool, swapchain_images.size(), command_buffers);

  for (uint32_t i = 0; i < swapchain_images.size(); i++)
  {
    SwapchainImage &swapchain_image = swapchain_images[i];

    VkCommandBuffer command_buffer = command_buffers[i];
    VkFramebuffer framebuffer = swapchain_image.vk_framebuffers[command_buffer_info.framebuffer];
    VkDescriptorSet descriptor_set = swapchain_image.vk_descriptor_sets[command_buffer_info.descriptor];

    FramebufferInfo framebuffer_info = framebuffer_infos[command_buffer_info.framebuffer];

    /* create command buffer begin info */
    VkCommandBufferBeginInfo command_buffer_begin_info = { };
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    /* begin command buffer */
    VkResult result = vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);
    if (result != VK_SUCCESS)
      throw exception("failed to begin command buffer [%s]", util::get_result_string(result));

    /* drawing commands */
    {
      /* create render pass begin info */
      VkRenderPassBeginInfo render_pass_begin_info = { };
      render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      render_pass_begin_info.pNext = nullptr;
      render_pass_begin_info.renderPass = vk_render_pass;
      render_pass_begin_info.framebuffer = framebuffer;
      render_pass_begin_info.renderArea.offset = {0, 0};
      render_pass_begin_info.renderArea.extent = vk_swapchain_image_extent;

      /* create clear values */
      uint32_t clear_value_count = 1;
      VkClearValue clear_values[clear_value_count];
      clear_values[0].color.float32[0] = framebuffer_info.clear_values[0];
      clear_values[0].color.float32[1] = framebuffer_info.clear_values[1];
      clear_values[0].color.float32[2] = framebuffer_info.clear_values[2];
      clear_values[0].color.float32[3] = framebuffer_info.clear_values[3];

      render_pass_begin_info.clearValueCount = clear_value_count;
      render_pass_begin_info.pClearValues = clear_values;

      /* begin recording */
      vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);

      /* draw every visible mesh */
      for (const MeshRef* mesh_ref : data_storage.meshes)
      {
        /* get vertex buffers and offsets */
        const size_t vertex_buffer_count = 1;
        VkBuffer vertex_buffers[vertex_buffer_count] = {mesh_ref->mesh->vertex_buffer};
        VkDeviceSize offsets[vertex_buffer_count] = {0};

        /* get index buffer */
        VkBuffer index_buffer = mesh_ref->mesh->index_buffer;

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);
        vkCmdBindVertexBuffers(command_buffer, 0, vertex_buffer_count, vertex_buffers, offsets);
        vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(mesh_ref->mesh->indices.size()), 1, 0, 0, 0);
      }

      /* stop recording */
      vkCmdEndRenderPass(command_buffer);
    }

    /* end command buffer */
    result = vkEndCommandBuffer(command_buffer);
    if (result != VK_SUCCESS)
      throw exception("failed to end command buffer [%s]", result);

    _command_buffer = command_buffer_infos.size();
    command_buffer_infos.push_back(command_buffer_info);
    swapchain_image.vk_command_buffers.push_back(command_buffer);
  }
  return 0;
}

int me::vulkan::Vulkan::cleanup_command_pool()
{
  vkDestroyCommandPool(vk_device, vk_graphics_command_pool, vk_allocation);
  vkDestroyCommandPool(vk_device, vk_transfer_command_pool, vk_allocation);
  return 0;
}

int me::vulkan::Vulkan::cleanup_command_buffers(const array_proxy<CommandBuffer, uint32_t> &_command_buffers)
{
  for (SwapchainImage &swapchain_image : swapchain_images)
  {
    VkCommandBuffer command_buffers[_command_buffers.size()];
    for (uint32_t i = 0; i < _command_buffers.size(); i++)
      command_buffers[i] = swapchain_image.vk_command_buffers[i];
    vkFreeCommandBuffers(vk_device, vk_graphics_command_pool, _command_buffers.size(), command_buffers);
  }
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
    throw me::exception("failed to allocate command buffers [%s]", me::vulkan::util::get_result_string(result));
  return 0;
}
