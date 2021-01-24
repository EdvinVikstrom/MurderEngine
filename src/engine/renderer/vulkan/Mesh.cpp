#include "Vulkan.hpp"
#include "Util.hpp"
#include "Memory.hpp"

static int create_vertex_buffer(
    VkPhysicalDevice 				physical_device,
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkDeviceMemory 				staging_buffer_memory,
    VkDeviceMemory 				vertex_buffer_memory,
    VkCommandPool 				transfer_command_pool,
    VkQueue 					transfer_queue,
    const me::vector<me::Vertex>		&vertices,
    VkBuffer 					&vertex_buffer
    );

static int create_index_buffer(
    VkPhysicalDevice 				physical_device,
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkDeviceMemory 				staging_buffer_memory,
    VkDeviceMemory 				index_buffer_memory,
    VkCommandPool 				transfer_command_pool,
    VkQueue 					transfer_queue,
    const me::vector<me::Index>			&indices,
    VkBuffer 					&index_buffer
    );


int me::vulkan::Vulkan::setup_mesh(const MeshInfo &mesh_info, Mesh* mesh)
{
  create_vertex_buffer(vk_physical_device, vk_device, vk_allocation,
      vk_staging_buffer_memory, vk_vertex_buffer_memory, vk_transfer_command_pool,
      queue_families.transfer_queue, mesh->vertices, mesh->vertex_buffer);
  create_index_buffer(vk_physical_device, vk_device, vk_allocation,
      vk_staging_buffer_memory, vk_index_buffer_memory, vk_transfer_command_pool,
      queue_families.transfer_queue, mesh->indices, mesh->index_buffer);
  return 0;
}

int me::vulkan::Vulkan::cleanup_mesh(Mesh* mesh)
{
  vkDestroyBuffer(vk_device, mesh->vertex_buffer, vk_allocation);
  vkDestroyBuffer(vk_device, mesh->index_buffer, vk_allocation);
  return 0;
}


int create_vertex_buffer(
    VkPhysicalDevice 				physical_device,
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkDeviceMemory 				staging_buffer_memory,
    VkDeviceMemory 				vertex_buffer_memory,
    VkCommandPool 				transfer_command_pool,
    VkQueue 					transfer_queue,
    const me::vector<me::Vertex>		&vertices,
    VkBuffer 					&vertex_buffer
    )
{
  VkDeviceSize buffer_size = vertices.size() * sizeof(me::Vertex);
  if (buffer_size == 0)
    throw me::exception("no vertices to allocate");

  /* create staging vertex buffer */
  VkBuffer staging_buffer;
  me::vulkan::memory::create_buffer(physical_device, device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      staging_buffer, staging_buffer_memory);

  /* copy data to staging buffer */
  me::vulkan::memory::map_buffer_memory(device, buffer_size, vertices.data(), staging_buffer_memory);

  /* create vertex buffer */
  me::vulkan::memory::create_buffer(physical_device, device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer, vertex_buffer_memory);

  /* copy buffer */
  me::vulkan::memory::copy_buffer(device, transfer_command_pool, transfer_queue, buffer_size, staging_buffer, vertex_buffer);

  /* destroy and free */
  vkDestroyBuffer(device, staging_buffer, allocation);
  vkFreeMemory(device, staging_buffer_memory, allocation);
  return 0;
}

int create_index_buffer(
    VkPhysicalDevice 				physical_device,
    VkDevice 					device,
    VkAllocationCallbacks* 			allocation,
    VkDeviceMemory 				staging_buffer_memory,
    VkDeviceMemory 				index_buffer_memory,
    VkCommandPool 				transfer_command_pool,
    VkQueue 					transfer_queue,
    const me::vector<me::Index>			&indices,
    VkBuffer 					&index_buffer
    )
{
  VkDeviceSize buffer_size = indices.size() * sizeof(me::Index);
  if (buffer_size == 0)
    throw me::exception("no indices to allocate");

  /* create staging vertex buffer */
  VkBuffer staging_buffer;
  me::vulkan::memory::create_buffer(physical_device, device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      staging_buffer, staging_buffer_memory);

  /* copy data to staging buffer */
  me::vulkan::memory::map_buffer_memory(device, buffer_size, indices.data(), staging_buffer_memory);

  /* create vertex buffer */
  me::vulkan::memory::create_buffer(physical_device, device, buffer_size,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer, index_buffer_memory);

  /* copy buffer */
  me::vulkan::memory::copy_buffer(device, transfer_command_pool, transfer_queue, buffer_size, staging_buffer, index_buffer);

  /* destroy and free */
  vkDestroyBuffer(device, staging_buffer, allocation);
  vkFreeMemory(device, staging_buffer_memory, allocation);
  return 0;
}
