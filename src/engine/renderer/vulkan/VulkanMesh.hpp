#include "Vulkan.inc"
#include <vulkan/vulkan_core.h>

#include <memory.h>

/* <--- SETUP ---> */
int me::Vulkan::setup_vertex_buffers()
{
  for (Mesh* mesh : data_storage.meshes)
    create_vertex_buffer(mesh);
  return 0;
}


/* <--- HELPERS ---> */
int me::Vulkan::create_vertex_buffer(Mesh* mesh)
{
  VkBufferCreateInfo buffer_create_info = { };
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.pNext = nullptr;
  buffer_create_info.flags = 0;
  buffer_create_info.size = sizeof(Vertex) * mesh->vertices.size();
  buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  buffer_create_info.queueFamilyIndexCount = 0;
  buffer_create_info.pQueueFamilyIndices = nullptr;

  VkResult result = vkCreateBuffer(logical_device_info.device, &buffer_create_info, nullptr, &mesh->vertex_buffer);
  if (result != VK_SUCCESS)
    throw exception("failed to create vertex buffer [%s]", vk_utils_result_string(result));

  VkMemoryRequirements memory_requirements;
  vkGetBufferMemoryRequirements(logical_device_info.device, mesh->vertex_buffer, &memory_requirements);

  uint32_t memory_type;
  get_memory_type(physical_device_info.device, memory_requirements.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memory_type);

  VkMemoryAllocateInfo vertex_buffer_memory_allocate_info = { };
  vertex_buffer_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  vertex_buffer_memory_allocate_info.pNext = nullptr;
  vertex_buffer_memory_allocate_info.allocationSize = memory_requirements.size;
  vertex_buffer_memory_allocate_info.memoryTypeIndex = memory_type;

  result = vkAllocateMemory(logical_device_info.device, &vertex_buffer_memory_allocate_info, nullptr, &memory_info.vertex_buffer_memory);
  if (result != VK_SUCCESS)
    throw exception("failed to allocate memory with mesh vertices(%lu) [%s]", mesh->vertices.size(), result);

  /* param[3]: offset */
  vkBindBufferMemory(logical_device_info.device, mesh->vertex_buffer, memory_info.vertex_buffer_memory, 0);


  void* data;
  result = vkMapMemory(logical_device_info.device, memory_info.vertex_buffer_memory, 0, buffer_create_info.size, 0, &data);
  if (result != VK_SUCCESS)
    throw exception("failed to map memory [%s]", vk_utils_result_string(result));
  
  memcpy(data, mesh->vertices.data(), buffer_create_info.size);

  vkUnmapMemory(logical_device_info.device, memory_info.vertex_buffer_memory);

  /* use flush instead */

  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_vertex_buffers()
{
  for (Mesh* mesh : data_storage.meshes)
    vkDestroyBuffer(logical_device_info.device, mesh->vertex_buffer, nullptr);
  return 0;
}
