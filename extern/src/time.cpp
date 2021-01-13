#include "time.hpp"

#include <sys/time.h>

using namespace me;

size_t time::microseconds()
{
  struct timeval time;
  gettimeofday(&time, nullptr);
  return time.tv_sec * 1000000 + time.tv_usec;
}

size_t time::milliseconds()
{
  return microseconds() / 1000L;
}
