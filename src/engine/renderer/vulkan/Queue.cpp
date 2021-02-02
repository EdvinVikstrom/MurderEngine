#include "Vulkan.hpp"
#include "Util.hpp"

#include <lme/algorithm.hpp>

int me::Vulkan::create_queue(const QueueCreateInfo &queue_create_info, Queue &queue)
{
  VERIFY_CREATE_INFO(queue_create_info, STRUCTURE_TYPE_QUEUE_CREATE_INFO);

  VkDevice vk_device = reinterpret_cast<Device_T*>(queue_create_info.device)->vk_device;
  uint32_t compute_queue_index = reinterpret_cast<Device_T*>(queue_create_info.device)->compute_queue_index;
  uint32_t graphics_queue_index = reinterpret_cast<Device_T*>(queue_create_info.device)->graphics_queue_index;
  uint32_t present_queue_index = reinterpret_cast<Device_T*>(queue_create_info.device)->present_queue_index;
  uint32_t transfer_queue_index = reinterpret_cast<Device_T*>(queue_create_info.device)->transfer_queue_index;

  VkQueue vk_queue;
  uint32_t vk_queue_index = UINT32_MAX;
  if (queue_create_info.queue_type == QUEUE_COMPUTE_TYPE)
    vk_queue_index = compute_queue_index;
  else if (queue_create_info.queue_type == QUEUE_GRAPHICS_TYPE)
    vk_queue_index = graphics_queue_index;
  else if (queue_create_info.queue_type == QUEUE_PRESENT_TYPE)
    vk_queue_index = present_queue_index;
  else if (queue_create_info.queue_type == QUEUE_TRANSFER_TYPE)
    vk_queue_index = transfer_queue_index;

  vkGetDeviceQueue(vk_device, vk_queue_index, 0, &vk_queue);
  queue = alloc.allocate<Queue_T>(vk_queue, vk_queue_index);
  return 0;
}
