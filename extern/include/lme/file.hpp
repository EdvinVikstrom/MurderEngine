#ifndef LIBME_FILE_HPP
  #define LIBME_FILE_HPP

#ifndef FILE_SEPARATOR
  #if (defined _WIN32 || defined _WIN64)
    #define FILE_SEPARATOR '\\'
  #else
    #define FILE_SEPARATOR '/'
  #endif
#endif

#include "type.hpp"
#include "string.hpp"

namespace me {

  class File {

  private:

    const string path;
    const uint8_t prefixLength;

  public:

    File(const string &path);
    File(const string &parent, const string &child);



    [[nodiscard]] const string get_absolute_path() const;
    [[nodiscard]] const string get_directory_path() const;

    int get_parent_path(string &path) const;

    int mkdir() const;
    int mkfile() const;

    [[nodiscard]] bool exists() const;
    [[nodiscard]] bool is_file() const;
    [[nodiscard]] bool is_absolute_path() const;
    [[nodiscard]] bool is_relative_path() const;
    [[nodiscard]] bool is_directory() const;

    [[nodiscard]] const string& get_path() const;
    [[nodiscard]] int get_type() const;

    size_t list_files(bool sub_dirs, int (*callback) (File&)) const;


    static int read(const File &file, size_t& size, char* &data);
    static int write(const File &file, size_t size, char* data);

    static const string normalize(const string &path);
    static const string join(const string &parent, const string &child);

    static string extension(const string &filename, const string &ext);

  };

}

#endif
