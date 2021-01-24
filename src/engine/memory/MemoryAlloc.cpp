#include "MemoryAlloc.hpp"

me::MemoryAlloc::MemoryAlloc()
  : stack{0, nullptr}
{
}

me::MemoryAlloc::MemoryAlloc(const size_t stack_length, size_t stack_position)
  : stack{stack_length, nullptr}
{
  stack.ptr = (char*) malloc(stack_length * sizeof(char));
  this->stack_position = stack_position;
}

me::MemoryAlloc::MemoryAlloc(const size_t length, char* ptr, size_t stack_position)
  : stack{length, ptr}
{
  this->stack_position = stack_position;
}


void* me::MemoryAlloc::begin() const
{
  return stack.ptr;
}

void* me::MemoryAlloc::end() const
{
  return stack.ptr + stack.size;
}

void* me::MemoryAlloc::allocate(size_t length, size_t size)
{
  length *= size;
  void* ptr = &stack.ptr[stack_position];
  printf("allocated [%lu] at %lu\n", length, stack_position);
  stack_position += length;

  return ptr;
}

void* me::MemoryAlloc::reallocate(void* ptr, size_t length, size_t size)
{
  throw exception("MemoryAlloc::reallocate() cannot reallocate stacked memory");
}

void me::MemoryAlloc::deallocate(void* ptr)
{
  throw exception("MemoryAlloc::deallocate() cannot deallocate stacked memory");
}

void me::MemoryAlloc::rewind(const size_t length)
{
  stack_position -= length;
}

me::MemoryAlloc me::MemoryAlloc::child(const size_t length) const
{
  MemoryAlloc memory_alloc(length, stack.ptr + stack_position);
  stack_position += length;
  return memory_alloc;
}
