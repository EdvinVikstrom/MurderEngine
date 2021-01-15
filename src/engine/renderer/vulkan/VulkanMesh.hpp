#include "Vulkan.inc"
#include <vulkan/vulkan_core.h>

/* <--- SETUP ---> */
int me::Vulkan::setup_vertex_buffers()
{
  logger.debug("> SETUP_VERTEX_BUFFERS");
  for (Mesh* mesh : data_storage.meshes)
  {
    create_vertex_buffer(mesh);
    create_index_buffer(mesh);
  }
  return 0;
}


/* <--- HELPERS ---> */
int me::Vulkan::create_vertex_buffer(Mesh* mesh)
{
  /* create staging vertex buffer */
  VkDeviceSize buffer_size = mesh->vertices.size() * sizeof(Vertex);
  VkBuffer staging_buffer;
  create_buffer(physical_device_info.device, logical_device_info.device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      staging_buffer, memory_info.staging_buffer_memory);

  /* copy data to staging buffer */
  map_buffer_memory(logical_device_info.device, buffer_size, mesh->vertices.data(), memory_info.staging_buffer_memory);

  /* create vertex buffer */
  create_buffer(physical_device_info.device, logical_device_info.device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mesh->vertex_buffer, memory_info.vertex_buffer_memory);

  /* copy buffer */
  copy_buffer(logical_device_info.device, command_pool_info.transfer_command_pool, queue_info.transfer_queue, buffer_size, staging_buffer, mesh->vertex_buffer);

  /* destroy and free */
  vkDestroyBuffer(logical_device_info.device, staging_buffer, nullptr);
  vkFreeMemory(logical_device_info.device, memory_info.staging_buffer_memory, nullptr);
  return 0;
}

int me::Vulkan::create_index_buffer(Mesh* mesh)
{
  /* create staging vertex buffer */
  VkDeviceSize buffer_size = mesh->indices.size() * sizeof(Index);
  VkBuffer staging_buffer;
  create_buffer(physical_device_info.device, logical_device_info.device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      staging_buffer, memory_info.staging_buffer_memory);

  /* copy data to staging buffer */
  map_buffer_memory(logical_device_info.device, buffer_size, mesh->indices.data(), memory_info.staging_buffer_memory);

  /* create vertex buffer */
  create_buffer(physical_device_info.device, logical_device_info.device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mesh->index_buffer, memory_info.index_buffer_memory);

  /* copy buffer */
  copy_buffer(logical_device_info.device, command_pool_info.transfer_command_pool, queue_info.transfer_queue, buffer_size, staging_buffer, mesh->index_buffer);

  /* destroy and free */
  vkDestroyBuffer(logical_device_info.device, staging_buffer, nullptr);
  vkFreeMemory(logical_device_info.device, memory_info.staging_buffer_memory, nullptr);
  return 0;
}


/* <--- CLEANUP ---> */
int me::Vulkan::cleanup_vertex_buffers()
{
  for (Mesh* mesh : data_storage.meshes)
  {
    vkDestroyBuffer(logical_device_info.device, mesh->vertex_buffer, nullptr);
    vkDestroyBuffer(logical_device_info.device, mesh->index_buffer, nullptr);
  }
  return 0;
}
