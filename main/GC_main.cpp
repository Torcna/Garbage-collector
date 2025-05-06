#include <iostream>

#include "High_lvl/GC_B.hpp"
#include "collector/collector_header.hpp"

struct MyStruct {
  int value;
  MyStruct(int v = 0) : value(v) {}
};

int main() {
  auto* memManager = new MemoryManager(16 * 1024 * 1024);  // Создаем в куче

  GC::garbageCollector.init(memManager);
  {
    auto* obj = memManagerNS::gc_new<MyStruct>(memManager, 42);
    std::cout << "Single object value: " << obj->value << std::endl;

    MyStruct* arr = memManagerNS::gc_new_array<MyStruct>(memManager, 5, 3);
    std::cout << "Array ptr is: " << arr << std::endl;
    for (std::size_t i = 0; i < 5; ++i) {
      std::cout << "arr[" << i << "].value = " << arr[i].value << std::endl;
    }
  }
  GC::garbageCollector.runGarbageCollector();
  return 0;
}