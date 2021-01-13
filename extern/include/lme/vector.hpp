#ifndef LIBME_VECTOR_HPP
  #define LIBME_VECTOR_HPP

#include "memory.hpp"

#include <alloca.h>
#include <stdlib.h>

namespace me {

  template<typename T>
  class vector {

  protected:

    template<typename Set_T> friend class set;

    typedef allocator alloc;

    size_t allocated;
    size_t length;
    T* elements;
    
  public:

    vector(size_t allocated, size_t length, T* elements)
      : allocated(allocated), length(length), elements(elements)
    {
    }

    vector(size_t length, const T &value)
      : vector(length, length, alloc::allocate<T>(sizeof(T), length))
    {
      for (size_t i = 0; i < length; i++)
	elements[i] = value;
    }

    vector(size_t length)
      : vector(length, length, alloc::allocate<T>(sizeof(T), length))
    {
      for (size_t i = 0; i < length; i++)
	elements[i] = T();
    }

    vector(const vector<T> &copy)
      : allocated(copy.allocated), length(copy.length), elements(alloc::allocate<T>(sizeof(T), length))
    {
      for (size_t i = 0; i < length; i++)
	this->elements[i] = copy.elements[i];
    }

    vector()
      : vector(0, 0, nullptr)
    {
    }

    ~vector()
    {
      alloc::deallocate<T>(elements);
    }


    [[nodiscard]] T* data() const
    {
      return elements;
    }

    [[nodiscard]] T* begin() const
    {
      return elements;
    }

    [[nodiscard]] T* end() const
    {
      return elements + length;
    }

    void reserve(size_t bytes)
    {
      if (bytes > allocated)
      {
	elements = alloc::reallocate<T>(elements, sizeof(T), bytes);
	allocated = bytes;
      }
    }


    void resize(size_t length, const T &value)
    {
      size_t begin = this->length;
      this->length = length;

      reserve(length);
      for (size_t i = begin; i < length; i++)
	elements[i] = value;
    }

    void resize(size_t length)
    {
      size_t begin = this->length;
      this->length = length;

      reserve(length);
      for (size_t i = begin; i < length; i++)
	elements[i] = T();
    }

    void push_back(T &&value)
    {
      reserve(allocated + 1);

      elements[length++] = static_cast<T&&>(value);
    }

    void push_back(const T &value)
    {
      reserve(allocated + 1);

      elements[length++] = value;
    }
    
    void push_back(const vector<T> &vec)
    {
      for (T &element : vec)
	push_back(element);
    }

    template<typename... A>
    T& emplace_back(A&&... args)
    {
      reserve(allocated + 1);
      return elements[length++] = T(static_cast<A&&>(args)...);
    }

    T pop_back()
    {
      length--;
      return elements[length];
    }

    void erase(T* first, T* last)
    {
      memory::move(last + 1, elements + length, first);
      length -= (last + 1 - first);
    }

    void erase(T* pos)
    {
      memory::move(pos + 1, elements + length, pos);
      length -= 1;
    }

    void clear()
    {
      length = 0;
    }


    [[nodiscard]] size_t find(T &&value) const
    {
      for (size_t i = 0; i < length; i++)
      {
	if (elements[i] == static_cast<T&&>(value))
	  return i;
      }
      return -1;
    }

    [[nodiscard]] size_t find(const T &value) const
    {
      for (size_t i = 0; i < length; i++)
      {
	if (elements[i] == value)
	  return i;
      }
      return -1;
    }

    [[nodiscard]] size_t rfind(T &&value) const
    {
      for (size_t i = length - 1; i; i--)
      {
	if (elements[i] == static_cast<T&&>(value))
	  return i;
      }
      return -1;
    }

    [[nodiscard]] size_t rfind(const T &value) const
    {
      for (size_t i = length - 1; i; i--)
      {
	if (elements[i] == value)
	  return i;
      }
      return -1;
    }

    [[nodiscard]] T& at(size_t index) const
    {
      return elements[index];
    }


    [[nodiscard]] size_t capacity() const
    {
      return allocated;
    }

    [[nodiscard]] size_t size() const
    {
      return length;
    }

    [[nodiscard]] bool is_empty() const
    {
      return length == 0;
    }


    [[nodiscard]] T& operator[](size_t index)
    {
      return elements[index];
    }


    [[nodiscard]] bool operator==(const vector& vec) const
    {
      if (vec.length != length)
	return false;

      for (size_t i = 0; i < length; i++)
      {
	if (vec.elements[i] != elements[i])
	  return false;
      }
      return true;
    }

    [[nodiscard]] bool operator!=(const vector& vec) const
    {
      if (vec.length == 0 && length == 0)
	return false;

      if (vec.length == 0 || length == 0)
	return true;

      return elements[0] != vec.elements[0];
    }

  };

}

#endif
