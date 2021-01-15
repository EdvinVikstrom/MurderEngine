#ifndef ME_MEMORY_ALLOC_HPP
  #define ME_MEMORY_ALLOC_HPP

#include "MemoryUtil.hpp"

#include <lme/vector.hpp>
#include <lme/pair.hpp>

namespace me {

  class MemoryAlloc {

  protected:

    MappedMemory stack;

    mutable size_t stack_position;

  public:

    MemoryAlloc();

    MemoryAlloc(const size_t stack_length, size_t stack_position = 0);

    MemoryAlloc(const size_t length, void** ptr, size_t stack_position = 0);

    template<typename T>
    [[nodiscard]] T* alloc(const size_t count)
    {
      const size_t length = count * sizeof(T);
      T* ptr = reinterpret_cast<T*>(stack.ptr[stack_position]);
      printf("allocated [%lu] at %lu\n", length, stack_position);
      stack_position += length;

      return ptr;
    }

    void rewind(const size_t length);

    MemoryAlloc child(const size_t length) const;

  };

}

#endif
