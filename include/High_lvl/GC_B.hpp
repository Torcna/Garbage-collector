#pragma once
#include <cstddef>
#include <utility>

#include "../High_lvl/Mem_manager.hpp"

namespace GC_B {

extern MemoryManager memManager;

// Функция для создания одного объекта
template <typename T, typename... Args>
T* gc_new(Args&&... args) {
  void* mem = memManager.allocate(sizeof(T));
  return new (mem) T(std::forward<Args>(args)...);
}
struct ArrayInfo {
  void* start;
  std::size_t count;
  std::size_t elem_size;
};

static std::vector<ArrayInfo>& getArrayTable() {
  static std::vector<ArrayInfo> table;
  return table;
}

template <typename T, typename... Args>
std::enable_if_t<std::is_default_constructible_v<T>, T*> gc_new_array(std::size_t count, Args&&... args) {
  if (count == 0) return nullptr;

  std::vector<T*> allocated;
  allocated.reserve(count);

  for (std::size_t i = 0; i < count; ++i) {
    void* mem = memManager.allocate(sizeof(T));
    if (!mem) {
      for (T* ptr : allocated) {
        memManager.deallocate(ptr, sizeof(T));
      }
      return nullptr;
    }
    T* current = new (mem) T(std::forward<Args>(args)...);
    allocated.push_back(current);

    if (i > 0) {
      T* prev = allocated[i - 1];
      if (reinterpret_cast<std::uintptr_t>(current) != reinterpret_cast<std::uintptr_t>(prev) + sizeof(T)) {
        for (T* ptr : allocated) {
          memManager.deallocate(ptr, sizeof(T));
        }
        return nullptr;
      }
    }
  }

  T* first = allocated[0];
  getArrayTable().emplace_back(ArrayInfo{first, count, sizeof(T)});

  return first;
}

}  // namespace GC_B