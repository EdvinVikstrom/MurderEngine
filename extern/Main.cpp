#include <stdio.h>
#include <unistd.h>

#include "vector.hpp"
#include "string.hpp"

int main()
{
  me::vector<int> test1;
  test1.push_back(1);
  test1.push_back(2);
  printf("pushed\n");
  me::vector<int> test2 = test1;
  test2.push_back(3);
  test2.push_back(4);
  test2.push_back(5);
  printf("pushed\n");

  printf("test1:\n");
  for (int const &i : test1)
    printf("  %i\n", i);

  printf("test2:\n");
  for (int const &i : test2)
    printf("  %i\n", i);
  return 0;
}
