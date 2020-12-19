#ifndef ME_VULKAN_ALLOC_HPP
  #define ME_VULKAN_ALLOC_HPP

namespace me {

  template<typename T>
  struct VkArray {

    uint32_t count;
    T* ptr;

    T& operator[](uint32_t index) const
    {
      return ptr[index];
    }

  };

  class VulkanAlloc {

  protected:

  public:

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
  array.ptr = new T[count];
  return array;
}

template<typename T>
me::VkArray<T>& me::VulkanAlloc::allocate_array(VkArray<T> &array)
{
  array.ptr = new T[array.count];
  return array;
}

#endif
