#ifndef ME_MEMORY_POOL_HPP
  #define ME_MEMORY_POOL_HPP

#include "MemoryAlloc.hpp"

namespace me {

  template<typename T>
  class MemoryPool {
  
  protected:

    MemoryAlloc allocator;

  public:

    explicit MemoryPool()
    {
    }

    explicit MemoryPool(MemoryAlloc &allocator)
      : allocator(allocator)
    {
    }
  
    T* allocate()
    {
      return allocator.allocate(1, sizeof(T));
    }

    void rewind(size_t length)
    {
      allocator.rewind(length);
    }
  
  };

}

#endif
