#ifndef LIBME_STATIC_VECTOR_HPP
  #define LIBME_STATIC_VECTOR_HPP

#include "memory.hpp"
#include "reference.hpp"

namespace me {

  template<typename T, size_t Size>
  class static_vector {

  protected:

    size_t length;
    T elements[Size];

  public:

    static_vector(size_t length, T* elements)
    {
      this->length = length;
      for (size_t i = 0; i < length; i++)
	this->elements[i] = elements[i];
    }

    static_vector(size_t length, T element)
    {
      this->length = length;
      for (size_t i = 0; i < length; i++)
	this->elements[i] = element;
    }

    constexpr static_vector(size_t length)
    {
      this->length = length;
    }

    constexpr static_vector(const static_vector<T, Size> &copy)
      : length(copy.length), elements(copy.elements)
    {
    }

    constexpr static_vector()
    {
      length = 0;
    }

    T* data() const
    {
      return elements;
    }

    T* begin() const
    {
      return elements;
    }

    T* end() const
    {
      return elements + length;
    }


    void push_back(T &&value)
    {
      elements[length++] = static_cast<T&&>(value);
    }

    void push_back(const T &value)
    {
      elements[length++] = value;
    }


    T& pop_back()
    {
      length--;
      return elements[length];
    }


    [[nodiscard]] T& at(size_t index) const
    {
      return elements[index];
    }

    [[nodiscard]] size_t size() const
    {
      return length;
    }

    [[nodiscard]] size_t capacity() const
    {
      return Size;
    }


    [[nodiscard]] T& operator[](size_t index) const
    {
      return elements[index];
    }


    [[nodiscard]] bool operator==(const static_vector &vec) const
    {
      if (vec.length != vec.length)
	return false;

      for (size_t i = 0; i < length; i++)
      {
	if (vec.elements[i] != elements[i])
	  return false;
      }
      return true;
    }

    [[nodiscard]] bool operator!=(const static_vector &vec) const
    {
      if (length == 0 && vec.length == 0)
	return false;

      if (length == 0 || vec.length == 0)
	return true;

      return elements[0] != vec.elements[0];
    }

  };

}

#endif
