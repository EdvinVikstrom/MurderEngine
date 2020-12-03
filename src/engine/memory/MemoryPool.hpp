#ifndef ME_MEMORY_POOL_HPP
  #define ME_MEMORY_POOL_HPP

namespace me {

  template<typename T>
  class MemPool {
  
  private:
  
    size_t size;
    T** pool;

  protected:

    size_t find(T* ptr);
  
  public:

    explicit MemPool(size_t size);
  
    T* alloc(size_t size);
    T* realloc(T* old_memory, size_t size);
    void free(T* memory);
  
  };

}

#include "MemoryPool_Impl.hpp"

#endif
