#ifndef ME_MEMORY_POOL_HPP
  #define ME_MEMORY_POOL_HPP

#include <lme/memory.hpp>

namespace me {

  template<typename T>
  class MemoryPool {
  
  protected:

    allocator alloc;

  public:

    explicit MemoryPool()
    {
    }

    explicit MemoryPool(allocator &alloc)
      : alloc(alloc)
    {
    }
  
    template<typename... A>
    [[nodiscard]] T* allocate(A&&... args)
    {
      return alloc.allocate<T>(static_cast<A&&>(args)...);
    }

    template<typename... A>
    [[nodiscard]] T* allocate(size_t count, A&&... args)
    {
      return alloc.allocate<T>(count, static_cast<A&&>(args)...);
    }

    [[nodiscard]] T* allocate(T &&value)
    {
      return alloc.allocate<T>(static_cast<T&&>(value));
    }

    [[nodiscard]] T* allocate(size_t count, T &&value)
    {
      return alloc.allocate<T>(count, static_cast<T&&>(value));
    }

    [[nodiscard]] T* reallocate(size_t count, T* ptr)
    {
      return alloc.reallocate<T>(count, ptr);
    }

    void deallocate(T* ptr)
    {
      alloc.deallocate(ptr);
    }
  
  };

}

#endif
