#include "string.hpp"

#include "string_impl.hpp"
#include "string_view_impl.hpp"

/* normal string */
#define CHAR_T char
  #include "temp_string.hpp"
  #include "temp_string_view.hpp"
#undef CHAR_T

/* wide string */
#define CHAR_T wchar_t
  #include "temp_string.hpp"
  #include "temp_string_view.hpp"
#undef CHAR_T


size_t me::strlen(const char* str)
{
  size_t length = 0;
  while (*str != 0)
  {
    str++;
    length++;
  }
  return length;
}

size_t me::strlen(const wchar_t* str)
{
  size_t length = 0;
  while (*str != 0)
  {
    str++;
    length++;
  }
  return length;
}

int me::strcmp(size_t len1, const char* str1, size_t len2, const char* str2)
{
  if (len1 != len2)
    return 1;
  
  for (size_t i = 0; i < len1; i++)
  {
    if (str1[i] != str2[i])
      return 1;
  }
  return 0;
}

int me::strcmp(const char* str1, const char* str2)
{
  return strcmp(strlen(str1), str1, strlen(str2), str2);
}

int me::strcmp(size_t len1, const wchar_t* str1, size_t len2, const wchar_t* str2)
{
  if (len1 != len2)
    return 1;
  
  for (size_t i = 0; i < len1; i++)
  {
    if (str1[i] != str2[i])
      return 1;
  }
  return 0;
}

int me::strcmp(const wchar_t* str1, const wchar_t* str2)
{
  return strcmp(strlen(str1), str1, strlen(str2), str2);
}
