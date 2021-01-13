#ifndef LIBME_MEMORY_HPP
  #define LIBME_MEMORY_HPP

#include "exception.hpp"

#include <stdlib.h>

namespace me {

  class allocator {

  public:

    template<typename T>
    static T* allocate(size_t size, size_t count)
    {
      return (T*) malloc(count * size);
    }

    template<typename T>
    static T* reallocate(T* ptr, size_t size, size_t count)
    {
      if (ptr == nullptr)
	return allocate<T>(size, count);
      return (T*) realloc(ptr, count * size);
    }

    template<typename T>
    static void deallocate(T* ptr)
    {
      free(ptr);
    }

  };

  template<typename T>
  class shared_ptr {

  private:

    mutable size_t* count;
    T* ptr;

  public:

    shared_ptr(size_t* count, T* ptr)
    {
      this->count = count;
      this->ptr = ptr;
    }

    shared_ptr(const shared_ptr &copy)
    {
      (*copy.count)++;

      this->count = copy.count;
      this->ptr = copy.ptr;
    }

    ~shared_ptr()
    {
      (*count)--;

      if (*count == 0)
	delete ptr;
    }


    [[nodiscard]] size_t ref_count() const
    {
      return *count;
    }

    [[nodiscard]] void* get_ptr() const
    {
      return ptr;
    }


    T* operator->() const
    {
      return ptr;
    }


    template<typename... A>
    [[nodiscard]] static shared_ptr make(A&&... args)
    {
      return shared_ptr(new size_t(1), new T(static_cast<A&&>(args)...));
    }

  };

  template<typename T>
  class unique_ptr {

  private:

    T* ptr;

  public:

    unique_ptr(T* ptr)
    {
      this->ptr = ptr;
    }

    unique_ptr(const unique_ptr &copy)
    {
      throw exception("unique_ptr cannot be copied");
    }


    T* operator->() const
    {
      return ptr;
    }


    template<typename... A>
    [[nodiscard]] static unique_ptr make(A&&... args)
    {
      return unique_ptr(new T(static_cast<A&&>(args)...));
    }

  };


  namespace memory {

    template<typename T>
    void move(T first, T last, T dest)
    {
      while (first != last)
	*dest++ = *first++;
    }

  }

} /* end namespace me::memory */

#endif
