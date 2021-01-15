#ifndef ME_VULKAN_ALLOC_HPP
  #define ME_VULKAN_ALLOC_HPP

#include <lme/array.hpp>

#include "../../memory/MemoryAlloc.hpp"

namespace me {

  template<typename T>
  struct VkArray : public array_t<T, uint32_t> {

  };

  class VulkanAlloc {

  protected:

    MemoryAlloc allocator;

  public:

    VulkanAlloc()
    {
    }

    VulkanAlloc(MemoryAlloc allocator)
      : allocator(allocator)
    {
    }

    template<typename T>
    VkArray<T>& allocate_array(uint32_t count, VkArray<T> &array);

    template<typename T>
    VkArray<T>& allocate_array(VkArray<T> &array);

  };

}


template<typename T>
me::VkArray<T>& me::VulkanAlloc::allocate_array(uint32_t count, VkArray<T> &array)
{
  array.count = count;
  //array.ptr = allocator.alloc<T>(count);
  array.ptr = new T[count];
  return array;
}

template<typename T>
me::VkArray<T>& me::VulkanAlloc::allocate_array(VkArray<T> &array)
{
  return allocate_array(array.count, array);
}

#endif
