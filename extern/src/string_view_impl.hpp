#include "template/string.hpp"

using namespace me;

template<typename T>
temp_string_view<T>::temp_string_view(const size_t length, const T* str)
  : length(length), str(str)
{
}

template<typename T>
temp_string_view<T>::temp_string_view(const T* str)
  : temp_string_view(strlen(str), str)
{
}

template<typename T>
temp_string_view<T>::temp_string_view(const temp_string_view<T> &str_view)
  : length(str_view.length), str(str_view.str)
{
}

template<typename T>
temp_string_view<T>::temp_string_view()
  : length(0), str(nullptr)
{
}

template<typename T>
size_t temp_string_view<T>::find(const T c) const
{
  for (size_t i = 0; i < length; i++)
  {
    if (c == str[i])
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string_view<T>::rfind(const T c) const
{
  for (size_t i = length - 1; i; i--)
  {
    if (c == str[i])
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string_view<T>::find(const size_t len, const T* c) const
{
  for (size_t i = 0; i < length; i++)
  {
    bool match = true;

    for (size_t j = 0; j < len; j++)
    {
      if ((i + j) >= length || str[j] != this->str[i + j])
      {
	match = false;
	break;
      }
    }

    if (match)
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string_view<T>::rfind(const size_t len, const T* c) const
{
  for (size_t i = length - 1; i; i--)
  {
    bool match = true;

    for (size_t j = 0; j < len; j++)
    {
      if ((i + j) >= length || str[j] != this->str[i + j])
      {
	match = false;
	break;
      }
    }

    if (match)
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string_view<T>::find(const temp_string_view<T> &str) const
{
  return find(str.length, str.str);
}

template<typename T>
size_t temp_string_view<T>::rfind(const temp_string_view<T> &str) const
{
  return rfind(str.length, str.str);
}

template<typename T>
void temp_string_view<T>::copy(size_t off, size_t len, T* dest) const
{
  for (size_t i = 0; i < len; i++)
    dest[i] = str[i + off];
}

template<typename T>
const T* temp_string_view<T>::c_str(T* str) const
{
  for (size_t i = 0; i < length; i++)
    str[i] = this->str[i];
  str[length] = 0;
  return str;
}

template<typename T>
size_t temp_string_view<T>::size() const
{
  return length;
}

template<typename T>
bool temp_string_view<T>::is_empty() const
{
  return length == 0;
}

template<typename T>
const T temp_string_view<T>::operator[](size_t index) const
{
  return str[index];
}

template<typename T>
bool temp_string_view<T>::operator==(const temp_string_view<T> &str) const
{
  if (str.length != length)
    return false;

  for (size_t i = 0; i < length; i++)
  {
    if (str.str[i] != this->str[i])
      return false;
  }
  return true;
}

template<typename T>
bool temp_string_view<T>::operator!=(const temp_string_view<T> &str) const
{
  return !operator==(str);
}

template<typename T>
bool temp_string_view<T>::operator>(const temp_string_view<T> &str) const
{
  return length > str.length;
}

template<typename T>
bool temp_string_view<T>::operator<(const temp_string_view<T> &str) const
{
  return length < str.length;
}

template<typename T>
bool temp_string_view<T>::operator>=(const temp_string_view<T> &str) const
{
  return length >= str.length;
}

template<typename T>
bool temp_string_view<T>::operator<=(const temp_string_view<T> &str) const
{
  return length <= str.length;
}
