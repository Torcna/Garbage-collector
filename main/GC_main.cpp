#include <iostream>
#include <vector>

#include "High_lvl/GC_B.hpp"
#include "collector/collector_header.hpp"

struct MyStruct {
  int value;
  MyStruct(int v = 0) : value(v) {}
};

void testf(MemoryManager* memManager) {
  auto* obj = memManagerNS::gc_new<MyStruct>(memManager, 42);
  std::cout << "Single object value: " << obj->value << std::endl;

  MyStruct* arr = memManagerNS::gc_new_array<MyStruct>(memManager, 5, 3);
  std::cout << "Array ptr is: " << arr << std::endl;
  for (std::size_t i = 0; i < 5; ++i) {
    std::cout << "arr[" << i << "].value = " << arr[i].value << std::endl;
  }
  char* arr2 = memManagerNS::gc_new_array<char>(memManager, 5);
  char* obj2 = memManagerNS::gc_new<char>(memManager, 'a');
  arr2[0] = 'a';
  arr2[1] = 'b';
  arr2[2] = 'c';
  for (std::size_t i = 0; i < 5; ++i) {
    std::cout << "arr2[" << i << "] = " << arr2[i] << std::endl;
  }
  std::cout << "obj2 value: " << *obj2 << std::endl;
  std::cout << "char* arr2 ptr is: " << static_cast<void*>(arr2) << std::endl;
  std::cout << "char* obj2 ptr is: " << obj2 << std::endl;
  char* arr3 = memManagerNS::gc_new_array<char>(memManager, 5);
  char* arr4 = memManagerNS::gc_new_array<char>(memManager, 5);
  std::cout << "char* arr3 ptr is: " << static_cast<void*>(arr3) << std::endl;
  std::cout << "char* arr4 ptr is: " << static_cast<void*>(arr4) << std::endl;
}

void fillChunk(MemoryManager* memManager) {
  constexpr size_t N = 2000;
  std::vector<MyStruct*> ptrs;
  ptrs.reserve(N);

  for (size_t i = 0; i < N; ++i) {
    auto* obj = memManagerNS::gc_new<MyStruct>(memManager, int(i));
    ptrs.push_back(obj);
  }
}
int main() {
  auto* memManager = new MemoryManager(16 * 1024 * 1024);

  GC::garbageCollector.init(memManager);
  fillChunk(memManager);
  GC::garbageCollector.runGarbageCollector();
  return 0;
}