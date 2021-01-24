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

    [[nodiscard]] T* begin() const
    {
      return (T*) allocator.begin();
    }

    [[nodiscard]] T* end() const
    {
      return (T*) allocator.end();
    }
  
    template<typename... A>
    [[nodiscard]] T* allocate(A&&... args)
    {
      return new T(static_cast<A&&>(args)...);
    }

    void rewind(size_t length)
    {
      allocator.rewind(length);
    }
  
  };

}

#endif
