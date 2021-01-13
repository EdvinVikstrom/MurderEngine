#include "template/string.hpp"

#define IS_UPPERCASE(c) (c >= 0x41 && c <= 0x5A)
#define IS_LOWERCASE(c) (c >= 0x61 && c <= 0x7A)

#define TO_UPPERCASE(c) IS_LOWERCASE(c) ? c - 0x20 : c;
#define TO_LOWERCASE(c) IS_UPPERCASE(c) ? c + 0x20 : c;

using namespace me;

template<typename T>
temp_string<T>::temp_string(size_t length, const T* str)
  : allocated(length + 1), length(length)
{
  this->str = alloc::allocate<T>(sizeof(T), allocated);
  for (size_t i = 0; i < length; i++)
    this->str[i] = str[i];
  this->str[length] = 0;
}

template<typename T>
temp_string<T>::temp_string(size_t length, const T c)
  : allocated(length + 1), length(length)
{
  this->str = alloc::allocate<T>(sizeof(T), allocated);
  for (size_t i = 0; i < length; i++)
    this->str[i] = c;
  this->str[length] = 0;
}

template<typename T>
temp_string<T>::temp_string(const T* str)
{
  length = strlen(str);
  allocated = length + 1;

  this->str = alloc::allocate<T>(sizeof(T), allocated);
  for (size_t i = 0; i < length; i++)
    this->str[i] = str[i];
  this->str[length] = 0;
}

template<typename T>
temp_string<T>::temp_string(const temp_string<T> &copy)
  : allocated(copy.allocated), length(copy.length), str(alloc::allocate<T>(sizeof(T), copy.allocated))
{
  for (size_t i = 0; i < length; i++)
    this->str[i] = copy.str[i];
  this->str[length]= 0;
}

template<typename T>
temp_string<T>::temp_string()
  : temp_string(0, '\0')
{
}

template<typename T>
temp_string<T>::~temp_string()
{
  alloc::deallocate<T>(str);
}


template<typename T>
T* temp_string<T>::data() const
{
  return str;
}

template<typename T>
T* temp_string<T>::begin() const
{
  return str;
}

template<typename T>
T* temp_string<T>::end() const
{
  return str + length;
}

template<typename T>
void temp_string<T>::reserve(size_t bytes)
{
  if (bytes > allocated)
  {
    str = alloc::reallocate<T>(str, sizeof(T), bytes);
    allocated = bytes;
  }
}

template<typename T>
void temp_string<T>::assign(size_t len, const T* str)
{
  reserve(len + 1);

  for (size_t i = 0; i < len; i++)
    this->str[i] = str[i];
  this->str[len] = 0;
  length = len;
}

template<typename T>
void temp_string<T>::assign(const temp_string<T> &str)
{
  assign(str.length, str.str);
}

template<typename T>
void temp_string<T>::append(size_t len, const T* str)
{
  reserve(length + len + 1);

  for (size_t i = 0; i < len; i++)
    this->str[length++] = str[i];
  this->str[length] = 0x00;
}

template<typename T>
void temp_string<T>::append(const temp_string<T> &str)
{
  append(str.length, str.str);
}

template<typename T>
void temp_string<T>::append(const T c)
{
  reserve(length + 2);

  str[length++] = c;
  str[length + 1] = 0x00;
}

template<typename T>
T temp_string<T>::pop_back()
{
  length--;
  T c = str[length];
  str[length] = 0x00;
  return c;
}

template<typename T>
void temp_string<T>::erase(T* first, T* last)
{
  memory::move(last + 1, str + length, first);
  this->length -= (last + 1 - first);
}

template<typename T>
void temp_string<T>::erase(T* pos)
{
  memory::move(pos + 1, str + length, pos);
  this->length -= 1;
}

template<typename T>
void temp_string<T>::replace(size_t first, size_t last, size_t len, const T* str)
{
}

template<typename T>
void temp_string<T>::replace(size_t first, size_t last, const temp_string<T> &str)
{
  replace(first, last, str.length, str.str);
}

template<typename T>
void temp_string<T>::to_lowercase(size_t off, size_t len)
{
  for (size_t i = off; i < len && i < length; i++)
    str[i] = TO_LOWERCASE(str[i]);
}

template<typename T>
void temp_string<T>::to_uppercase(size_t off, size_t len)
{
  for (size_t i = off; i < len && i < length; i++)
    str[i] = TO_UPPERCASE(str[i]);
}

template<typename T>
T& temp_string<T>::at(size_t index) const
{
  return str[index];
}

template<typename T>
size_t temp_string<T>::find(const T c) const
{
  for (size_t i = 0; i < length; i++)
  {
    if (c == str[i])
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string<T>::rfind(const T c) const
{
  for (size_t i = length - 1; i; i--)
  {
    if (c == str[i])
      return i;
  }
  return -1;
}

template<typename T>
size_t temp_string<T>::find(size_t len, const T* str) const
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
size_t temp_string<T>::rfind(size_t len, const T* str) const
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
size_t temp_string<T>::find(const temp_string<T> &str) const
{
  return find(str.length, str.str);
}

template<typename T>
size_t temp_string<T>::rfind(const temp_string<T> &str) const
{
  return rfind(str.length, str.str);
}

template<typename T>
temp_string<T> temp_string<T>::substr(size_t begin, size_t end) const
{
  return temp_string<T>(end - begin, str + begin);
}

template<typename T>
temp_string<T> temp_string<T>::substr(size_t begin) const
{
  return temp_string<T>(length - begin, str + begin);
}

template<typename T>
void temp_string<T>::copy(size_t off, size_t len, T* dest) const
{
  for (size_t i = 0; i < len; i++)
    dest[i] = str[i + off];
}

template<typename T>
const T* temp_string<T>::c_str() const
{
  return str;
}

template<typename T>
size_t temp_string<T>::capacity() const
{
  return allocated;
}

template<typename T>
size_t temp_string<T>::size() const
{
  return length;
}

template<typename T>
bool temp_string<T>::is_empty() const
{
  return length == 0;
}

template<typename T>
temp_string<T>& temp_string<T>::operator+=(const T c)
{
  append(c);
  return *this;
}

template<typename T>
temp_string<T>& temp_string<T>::operator+=(const temp_string<T> &str)
{
  append(str);
  return *this;
}

template<typename T>
temp_string<T>& temp_string<T>::operator*=(size_t count)
{
  size_t length = this->length;
  size_t new_length = length * count;
  reserve(new_length + 1);

  for (size_t i = length; i < new_length; i++)
    str[i] = str[(i - length) % length];
  str[new_length] = 0x00;
  return *this;
}

template<typename T>
T& temp_string<T>::operator[](size_t index) const
{
  return str[index];
}

template<typename T>
bool temp_string<T>::operator==(const temp_string<T> &str) const
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
bool temp_string<T>::operator!=(const temp_string<T> &str) const
{
  return !operator==(str);
}

template<typename T>
bool temp_string<T>::operator>(const temp_string<T> &str) const
{
  return length > str.length;
}

template<typename T>
bool temp_string<T>::operator<(const temp_string<T> &str) const
{
  return length < str.length;
}

template<typename T>
bool temp_string<T>::operator>=(const temp_string<T> &str) const
{
  return length >= str.length;
}

template<typename T>
bool temp_string<T>::operator<=(const temp_string<T> &str) const
{
  return length <= str.length;
}
