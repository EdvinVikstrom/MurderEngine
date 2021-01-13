#ifndef LIBME_SET_HPP
  #define LIBME_SET_HPP

#include "vector.hpp"

namespace me {

  template<typename T>
  class set : public vector<T> /* WIP */ {

  public:

    set(const size_t len, const T* elements)
    {
      for (size_t i = 0; i < len; i++)
      {
	if (this->find(elements[i]) == -1)
	  this->push_back(elements[i]);
      }
    }

    set(vector<T> &elements)
    {
      for (T &element : elements)
      {
	if (this->find(element) == -1)
	  this->push_back(element);
      }
    }


    void push_back(T &&value)
    {
      size_t index = this->find(value);
      if (index == -1)
	vector<T>::push_back(value);
    }

    void push_back(const T &value)
    {
      size_t index = this->find(value);
      if (index == -1)
	vector<T>::push_back(value);
    }

  };

}

#endif
