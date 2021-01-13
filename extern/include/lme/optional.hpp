#ifndef LIBME_OPTIONAL_HPP
  #define LIBME_OPTIONAL_HPP

#include "exception.hpp"

namespace me {

  template<typename T>
  class optional {

  private:

    bool valid;
    T data;

  public:

    constexpr optional()
      : valid(false)
    {
    }

    bool has() const
    {
      return valid;
    }

    T& value()
    {
      if (!valid)
	throw exception("me::optional<T>::data not valid");
      return data;
    }

    void assign(T &&value)
    {
      data = static_cast<T&&>(value);
      valid = true;
    }

    void assign(const T &value)
    {
      data = value;
      valid = true;
    }


    void operator=(T &&value)
    {
      assign(value);
    }

    void operator=(const T &value)
    {
      assign(value);
    }

  };

}

#endif
