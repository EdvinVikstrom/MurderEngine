#ifndef LIBME_EXCEPTION_HPP
  #define LIBME_EXCEPTION_HPP

#include "type.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace me {

  class exception {

  private:

    char message[1024];

  public:

    exception(const char* format, ...)
    {
      va_list args;
      va_start(args, format);
      vsprintf(message, format, args);
      va_end(args);
    }

    [[nodiscard]] const char* get_message() const
    {
      return message;
    }
    
    const char* what() const throw()
    {
      return message;
    }

  };

}

#endif
