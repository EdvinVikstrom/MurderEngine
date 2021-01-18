#ifndef ME_MEMORY_ALLOC_HPP
  #define ME_MEMORY_ALLOC_HPP

#include "MemoryUtil.hpp"

#include <lme/vector.hpp>
#include <lme/pair.hpp>

namespace me {

  class MemoryAlloc : public allocator {

  protected:

    MappedMemory stack;

    mutable size_t stack_position;

  public:

    MemoryAlloc();

    MemoryAlloc(const size_t stack_length, size_t stack_position = 0);

    MemoryAlloc(const size_t length, void** ptr, size_t stack_position = 0);

    [[nodiscard]] void* allocate(size_t length, size_t size) override
    {
      length *= size;
      void* ptr = stack.ptr[stack_position];
      printf("allocated [%lu] at %lu\n", length, stack_position);
      stack_position += length;

      return ptr;
    }

    [[nodiscard]] void* reallocate(void* ptr, size_t length, size_t size) override
    {
      throw exception("MemoryAlloc::reallocate() cannot reallocate stacked memory");
    }

    void deallocate(void* ptr) override
    {
      throw exception("MemoryAlloc::deallocate() cannot deallocate stacked memory");
    }

    void rewind(const size_t length);

    MemoryAlloc child(const size_t length) const;

  };

}

#endif
