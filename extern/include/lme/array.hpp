#ifndef LIBME_ARRAY_HPP
  #define LIBME_ARRAY_HPP

namespace me {

  template<typename T, size_t SIZE>
  class array {

  public:

    typedef T Type[SIZE];
    Type elements;

    [[nodiscard]] T* begin() const
    {
      return (T*) elements;
    }

    [[nodiscard]] T* end() const
    {
      return (T*) elements + SIZE;
    }


    void swap(size_t src, size_t dst)
    {
      T &temp = elements[src];
      elements[src] = elements[dst];
      elements[dst] = temp;
    }


    void copy(size_t off, size_t len, T* dest) const
    {
      for (size_t i = 0; i < len; i++)
	dest[i] = elements[i + off];
    }


    [[nodiscard]] T& at(size_t index) const
    {
      return elements[index];
    }


    [[nodiscard]] size_t size() const
    {
      return SIZE;
    }


    [[nodiscard]] T& operator[](size_t index) const
    {
      return elements[index];
    }


    [[nodiscard]] bool operator==(const array<T, SIZE>& arr) const
    {
      for (size_t i = 0; i < SIZE; i++)
      {
	if (arr.elements[i] != elements[i])
	  return false;
      }
      return true;
    }

    [[nodiscard]] bool operator!=(const array<T, SIZE>& arr) const
    {
      if (SIZE == 0)
	return false;

      return elements[0] != arr.elements[0];
    }

  };

}

#endif
