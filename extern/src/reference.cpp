#include "reference.hpp"

using namespace me;

size_t* reference::make_counter()
{
  return new size_t(1);
}

void reference::free_counter(size_t* counter)
{
  delete counter;
}
