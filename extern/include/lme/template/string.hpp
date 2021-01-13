#ifndef LIBME_BITS_STRING_HPP
  #define LIBME_BITS_STRING_HPP

#include "../memory.hpp"

namespace me {

  template<typename T>
  class temp_string {
  
  protected:

    typedef allocator alloc;

    size_t allocated;
    size_t length;
    T* str;
  
  public:
  
    temp_string(size_t length, const T* str);
    temp_string(size_t length, const T c);
    temp_string(const T* str);
    temp_string(const temp_string<T> &copy);
    temp_string();
    ~temp_string();
  
    [[nodiscard]] T* data() const;
    [[nodiscard]] T* begin() const;
    [[nodiscard]] T* end() const;
    void reserve(size_t bytes);
 
    void assign(size_t len, const T* str);
    void assign(const temp_string &str);
    void append(size_t len, const T* str);
    void append(const temp_string &str);
    void append(const T c);
    T pop_back();
    [[deprecated]] void erase(T* first, T* last);
    [[deprecated]] void erase(T* pos);
    void replace(size_t first, size_t last, size_t len, const T* str);
    void replace(size_t first, size_t last, const temp_string &str);
    void to_lowercase(size_t off = 0, size_t len = -1);
    void to_uppercase(size_t off = 0, size_t len = -1);
 
    [[nodiscard]] T& at(size_t index) const;
 
    [[nodiscard]] size_t find(const T c) const;
    [[nodiscard]] size_t rfind(const T c) const;
    [[nodiscard]] size_t find(size_t len, const T* str) const;
    [[nodiscard]] size_t rfind(size_t len, const T* str) const;
    [[nodiscard]] size_t find(const temp_string &str) const;
    [[nodiscard]] size_t rfind(const temp_string &str) const;
    [[nodiscard]] temp_string substr(size_t begin, size_t end) const;
    [[nodiscard]] temp_string substr(size_t begin) const;
    void copy(size_t off, size_t len, T* dest) const;
  
    [[nodiscard]] const T* c_str() const;
    [[nodiscard]] size_t capacity() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool is_empty() const;

  
    temp_string& operator+=(const T c);
    temp_string& operator+=(const temp_string &str);
    temp_string& operator*=(size_t count);
  
    [[nodiscard]] T& operator[](size_t index) const;
  
    [[nodiscard]] bool operator==(const temp_string &str) const;
    [[nodiscard]] bool operator!=(const temp_string &str) const;
    [[nodiscard]] bool operator>(const temp_string &str) const;
    [[nodiscard]] bool operator<(const temp_string &str) const;
    [[nodiscard]] bool operator>=(const temp_string &str) const;
    [[nodiscard]] bool operator<=(const temp_string &str) const;
 
  };

  template<typename T>
  class temp_string_view {

  protected:

    const size_t length;
    const T* str;

  public:

    temp_string_view(const size_t length, const T* str);
    temp_string_view(const T* str);
    temp_string_view(const temp_string_view<T> &str_view);
    temp_string_view();

    [[nodiscard]] size_t find(const T c) const;
    [[nodiscard]] size_t rfind(const T c) const;
    [[nodiscard]] size_t find(const size_t len, const T* c) const;
    [[nodiscard]] size_t rfind(const size_t len, const T* c) const;
    [[nodiscard]] size_t find(const temp_string_view &str) const;
    [[nodiscard]] size_t rfind(const temp_string_view &str) const;
    void copy(size_t off, size_t len, T* dest) const;

    [[nodiscard]] const T* c_str(T* str) const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool is_empty() const;

    [[nodiscard]] const T operator[](size_t index) const;

    [[nodiscard]] bool operator==(const temp_string_view &str) const;
    [[nodiscard]] bool operator!=(const temp_string_view &str) const;
    [[nodiscard]] bool operator>(const temp_string_view &str) const;
    [[nodiscard]] bool operator<(const temp_string_view &str) const;
    [[nodiscard]] bool operator>=(const temp_string_view &str) const;
    [[nodiscard]] bool operator<=(const temp_string_view &str) const;

  };


  [[nodiscard]] size_t strlen(const char* str);
  [[nodiscard]] size_t strlen(const wchar_t* str);

  [[nodiscard]] int strcmp(size_t len1, const char* str1, size_t len2, const char* str2);
  [[nodiscard]] int strcmp(const char* str1, const char* str2);
  [[nodiscard]] int strcmp(size_t len1, const wchar_t* str1, size_t len2, const wchar_t* str2);
  [[nodiscard]] int strcmp(const wchar_t* str1, const wchar_t* str2);


  template<typename C>
  [[nodiscard]] static temp_string<C> operator+(const temp_string<C> &str1, const temp_string<C> &str2);

  template<typename C>
  [[nodiscard]] static temp_string<C> operator*(const temp_string<C> &str1, size_t count);

  template<typename C>
  temp_string<C> operator+(const temp_string<C> &str1, const temp_string<C> &str2)
  {
    temp_string<C> str = str1;
    str += str2;
    return str;
  }

  template<typename C>
  temp_string<C> operator*(const temp_string<C> &str1, size_t count)
  {
    temp_string<C> str = str1;
    str *= count;
    return str;
  }
  
}

#endif
