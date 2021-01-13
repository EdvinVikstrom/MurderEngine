#ifndef LIBME_ALGORITHM_HPP
  #define LIBME_ALGORITHM_HPP

namespace me {

  template<typename T>
  size_t array_find(size_t len, T* array, T &t)
  {
    for (size_t i = 0; i < len; i++)
    {
      if (array[i] == t)
	return i;
    }
    return -1;
  }

}

#endif
