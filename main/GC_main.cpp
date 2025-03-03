#include <iostream>

#include "High_lvl/GC_B.hpp"

struct MyStruct {
  int value;
  MyStruct(int v = 0) : value(v) {}
};

int main() {
  MyStruct* obj = GC_B::gc_new<MyStruct>(42);
  std::cout << "Single object value: " << obj->value << std::endl;

  MyStruct* arr = GC_B::gc_new_array<MyStruct>(5, 3);
  for (std::size_t i = 0; i < 5; ++i) {
    std::cout << "arr[" << i << "].value = " << arr[i].value << std::endl;
  }

  return 0;
}