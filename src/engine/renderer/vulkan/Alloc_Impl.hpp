#include "VulkanRenderer.hpp"

void* me::VulkanRenderer::vk_allocate(void* user_data, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
  MemPool<void>* mem_pool = (MemPool<void>*) user_data;
  return mem_pool->alloc(size);
}

void* me::VulkanRenderer::vk_reallocate(void* user_data, void* original, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
  MemPool<void>* mem_pool = (MemPool<void>*) user_data;
  return mem_pool->realloc(original, size);
}

void me::VulkanRenderer::vk_free(void* user_data, void* memory)
{
  MemPool<void>* mem_pool = (MemPool<void>*) user_data;
  return mem_pool->free(memory);
}

void me::VulkanRenderer::vk_iallocaten(void* user_data, size_t size, VkInternalAllocationType alloc_type, VkSystemAllocationScope scope)
{
}

void me::VulkanRenderer::vk_ifreen(void* user_data, size_t size, VkInternalAllocationType alloc_type, VkSystemAllocationScope scope)
{
}
