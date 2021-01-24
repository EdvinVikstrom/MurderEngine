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

    MemoryAlloc(const size_t length, char* ptr, size_t stack_position = 0);

    [[nodiscard]] void* begin() const;
    [[nodiscard]] void* end() const;

    [[nodiscard]] void* allocate(size_t length, size_t size) override;
    [[nodiscard]] void* reallocate(void* ptr, size_t length, size_t size) override;
    void deallocate(void* ptr) override;

    void rewind(const size_t length);

    MemoryAlloc child(const size_t length) const;

  };

}

#endif
