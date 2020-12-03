#include <cstdlib>

template<typename T>
me::MemPool<T>::MemPool(size_t size)
{
  this->size = size;
  pool = (T**) ::calloc(size, sizeof(T*));
}

template<typename T>
me::size_t me::MemPool<T>::find(T* ptr)
{
  for (size_t i = 0; i < size; i++)
  {
    if (pool[i] == ptr)
      return i;
  }
  return -1;
}

template<typename T>
T* me::MemPool<T>::alloc(size_t size)
{
  T* memory = (T*) ::malloc(size);
  pool[find(nullptr)] = memory;
  return memory;
}

template<typename T>
T* me::MemPool<T>::realloc(T* old_memory, size_t size)
{
  return pool[find(old_memory)] = (T*) ::realloc((void*) old_memory, size);
}

template<typename T>
void me::MemPool<T>::free(T* memory)
{
  ::free(memory);
  pool[find(memory)] = nullptr;
}
