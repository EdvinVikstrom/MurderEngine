#include "MemoryAlloc.hpp"

me::MemoryAlloc::MemoryAlloc()
  : stack{0, nullptr}
{
}

me::MemoryAlloc::MemoryAlloc(const size_t stack_length, size_t stack_position)
  : stack{stack_length, nullptr}
{
  stack.ptr = reinterpret_cast<void**>(malloc(stack_length * sizeof(void*)));
  this->stack_position = stack_position;
}

me::MemoryAlloc::MemoryAlloc(const size_t length, void** ptr, size_t stack_position)
  : stack{length, ptr}
{
  this->stack_position = stack_position;
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
