#ifndef LIBME_STRING_HPP
  #define LIBME_STRING_HPP

#include "template/string.hpp"

#include <wchar.h>

namespace me {

  typedef temp_string<char> string;
  typedef temp_string<wchar_t> wstring;

  typedef temp_string_view<char> string_view;
  typedef temp_string_view<wchar_t> wstring_view;

}

#endif
