#include "VulkanRenderer.hpp"

int me::VulkanRenderer::init_commands()
{
  VkCommandPoolCreateInfo command_pool_info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueFamilyIndex = queue_family_indices.graphics_family.value()
  };

  VkResult result = vkCreateCommandPool(device, &command_pool_info, &alloc_callbacks, &command_pool);
  if (result != VK_SUCCESS)
    vk_error(result);

  return 0;
}

int me::VulkanRenderer::free_command_buffers()
{
  return 0;
}
