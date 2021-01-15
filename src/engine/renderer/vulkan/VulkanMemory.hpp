#include "Vulkan.inc"
#include <vulkan/vulkan_core.h>

/* <--- SETUP ---> */
int me::Vulkan::setup_memory()
{
  logger.debug("> SETUP_MEMORY");
  return 0;
}

int me::Vulkan::setup_synchronization()
{
  logger.debug("> SETUP_SYNCHRONIZATION");

  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.image_available);
  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.render_finished);
  alloc.allocate_array(MAX_FRAMES_IN_FLIGHT, synchronization_info.in_flight);
  alloc.allocate_array(swapchain_info.images.count, synchronization_info.images_in_flight);

  VkSemaphoreCreateInfo semaphore_create_info = { };
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = 0;

  VkFenceCreateInfo fence_create_info = { };
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < synchronization_info.images_in_flight.count; i++)
    synchronization_info.images_in_flight[i] = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    /* create 'image available' semaphore */
    VkResult result = vkCreateSemaphore(logical_device_info.device, &semaphore_create_info, nullptr, &synchronization_info.image_available[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", vk_utils_result_string(result));

    /* create 'render finished' semaphore */
    result = vkCreateSemaphore(logical_device_info.device, &semaphore_create_info, nullptr, &synchronization_info.render_finished[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create semaphore [%s]", vk_utils_result_string(result));

    /* create 'in flight' fence */
    result = vkCreateFence(logical_device_info.device, &fence_create_info, nullptr, &synchronization_info.in_flight[i]);
    if (result != VK_SUCCESS)
      throw exception("failed to create fence [%s]", vk_utils_result_string(result));
  }
  return 0;
}


/* <--- HELPERS ---> */
int me::Vulkan::get_memory_type(VkPhysicalDevice physical_device,
    uint32_t type_filter,
    VkMemoryPropertyFlags memory_property_flags,
    uint32_t &memory_type)
{
  VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
  vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

  for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; i++)
  {
    if (type_filter & (1 << i) &&
	(physical_device_memory_properties.memoryTypes[i].propertyFlags & memory_property_flags) == memory_property_flags)
    {
      memory_type = i;
      return 0;
    }
  }

  throw exception("failed to find suitable memory type");
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_memory()
{
  vkFreeMemory(logical_device_info.device, memory_info.vertex_buffer_memory, nullptr);
  return 0;
}

int me::Vulkan::cleanup_synchronization()
{
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
  {
    vkDestroySemaphore(logical_device_info.device, synchronization_info.image_available[i], nullptr);
    vkDestroySemaphore(logical_device_info.device, synchronization_info.render_finished[i], nullptr);
    vkDestroyFence(logical_device_info.device, synchronization_info.in_flight[i], nullptr);
  }
  return 0;
}
