#include "file.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

using namespace me;

File::File(const string &path)
  : path(normalize(path)), prefixLength(this->path.size() > 0 && this->path.at(0) == FILE_SEPARATOR ? 1 : 0)
{
}

File::File(const string &parent, const string &child)
  : File(join(parent, child))
{
}

const string File::get_absolute_path() const
{
  string real_path(PATH_MAX, '\0');
  realpath(path.c_str(), real_path.data());
  return real_path;
}

const string File::get_directory_path() const
{
  size_t index = path.rfind(FILE_SEPARATOR);

  if (index == -1)
    return ".";
  else if (index == 0)
    return string(1, FILE_SEPARATOR);

  return path.substr(0, index);
}

int File::get_parent_path(string &path) const
{
  size_t index = this->path.rfind(FILE_SEPARATOR);

  if (index == -1 || index < prefixLength)
  {
    if (prefixLength > 0)
    {
      path.assign(this->path.substr(0, prefixLength));
      return 1;
    }
    return 0;
  }

  path.assign(this->path.substr(0, index));
  return 1;
}

int File::mkdir() const
{
  return ::mkdir(path.c_str(), 0700) == 0;
}

int File::mkfile() const
{
  write(*this, 0, new char[0]);
  return 1;
}

bool File::exists() const
{
  return access(path.c_str(), R_OK) == 0;
}

bool File::is_file() const
{
  if (!exists())
    return false;
  return S_ISREG(get_type());
}

bool File::is_absolute_path() const
{
  return prefixLength > 0;
}

bool File::is_relative_path() const
{
  char cwd[PATH_MAX + 1];
  getcwd(cwd, PATH_MAX + 1);
  size_t len = strlen(cwd);

  string abs = get_absolute_path();
  return abs.size() >= len && abs.find(cwd) == 0;
}

bool File::is_directory() const
{
  if (!exists())
    return false;
  return S_ISDIR(get_type());
}

const string& File::get_path() const
{
  return path;
}

int File::get_type() const
{
  struct stat stat_path;
  stat(path.c_str(), &stat_path);
  return stat_path.st_mode;
}

size_t File::list_files(bool sub_dirs, int (*callback) (File&)) const
{
  if (!is_directory())
    return 0;

  size_t count = 0;

  DIR* dir;
  if ((dir = opendir(path.c_str())) != nullptr)
  {
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
	continue;

      File file(path, entry->d_name);

      if (sub_dirs && file.is_directory())
	count += file.list_files(sub_dirs, callback);
      else if (file.is_file())
      {
	count++;
	if (!callback(file))
	  break;
      }
    }
    closedir(dir);
  }

  return count;
}


int File::read(const File &file, size_t &size, char* &data)
{
  FILE* f = fopen(file.get_path().c_str(), "rb");
  if (f == NULL)
    throw exception("failed to open file '%s': [%s]", file.get_path().c_str(), strerror(errno));

  fseek(f, 0L, SEEK_END);
  size = ftell(f);

  rewind(f);

  data = (char*) malloc(size);
  fread(data, 1, size, f);

  fclose(f);

  return 0;
}

int File::write(const File &file, size_t size, char* data)
{
  FILE* f = fopen(file.get_path().c_str(), "wb");
  if (f == NULL)
    throw exception("failed to write file '%s': [%s]", file.get_path().c_str(), strerror(errno));

  fwrite(data, 1, size, f);

  fclose(f);

  return 1;
}

const string File::normalize(const string &path)
{
  string fixed;
  fixed.reserve(path.size() + 1);

  for (uint32_t i = 0; i < path.size(); i++)
  {
    char c = path.at(i);

    if (i > 0 && c == FILE_SEPARATOR && path.at(i - 1) == FILE_SEPARATOR)
      continue;

    if (i > 2 && i < path.size() - 1 && c == '.' && path[i + 1] == FILE_SEPARATOR)
    {
      i++;
      continue;
    }

    fixed += c;
  }

  return fixed;
}

const string File::join(const string &parent, const string &child)
{
  string path = parent;
  
  if (path.size() > 0 && path.at(path.size() - 1) != FILE_SEPARATOR)
    path += FILE_SEPARATOR;

  path.append(child);
  return path;
}

/*
 * '/home/me/.stuff/testcpp' gonna explode [fixed?]
 */
string File::extension(const string &filename, const string &ext)
{
  string new_str;
  new_str.reserve(PATH_MAX);
  size_t index = filename.rfind('.');
  size_t index2 = filename.rfind(FILE_SEPARATOR);

  if (index != -1 && index2 != -1 && index < index2)
    index = -1;

  for (uint32_t i = 0; i < (index == -1 ? filename.size() : index); i++)
    new_str += filename.at(i);

  new_str += '.';
  new_str.append(ext);
  return new_str;
}
