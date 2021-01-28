#include "Vulkan.hpp"
#include "Util.hpp"

#include <lme/algorithm.hpp>

int me::Vulkan::create_queue(const QueueCreateInfo &queue_create_info, Queue &queue)
{
  VERIFY_CREATE_INFO(queue_create_info, STRUCTURE_TYPE_QUEUE_CREATE_INFO);

  VulkanDevice* device = reinterpret_cast<VulkanDevice*>(queue_create_info.device);

  VkQueue vk_queue;
  uint32_t vk_queue_index = UINT32_MAX;
  if (queue_create_info.queue_type == QUEUE_COMPUTE_TYPE)
    vk_queue_index = device->compute_queue_index;
  else if (queue_create_info.queue_type == QUEUE_GRAPHICS_TYPE)
    vk_queue_index = device->graphics_queue_index;
  else if (queue_create_info.queue_type == QUEUE_PRESENT_TYPE)
    vk_queue_index = device->present_queue_index;
  else if (queue_create_info.queue_type == QUEUE_TRANSFER_TYPE)
    vk_queue_index = device->transfer_queue_index;

  vkGetDeviceQueue(device->vk_device, vk_queue_index, 0, &vk_queue);
  queue = alloc.allocate<VulkanQueue>(vk_queue, vk_queue_index);
  return 0;
}
