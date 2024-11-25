#include "header.hpp"

Allocator instance__;

int main() {


  long long int* a = static_cast<long long int*>(instance__.allocate<long long int>());
  double* b = static_cast<double*>(instance__.allocate<double>());
  *a = 42;
  *b = 3.14;
  instance__.deallocate<long long int>(a);
  instance__.deallocate<long long int>(b);
  return 0;
}