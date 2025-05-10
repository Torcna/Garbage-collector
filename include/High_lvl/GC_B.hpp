#pragma once
#include <cmath>
#include <cstddef>
#include <utility>

#include "High_lvl/Mem_manager.hpp"

namespace memManagerNS {

template <typename T, typename... Args>
T* gc_new(MemoryManager* memManager, Args&&... args) {
  void* mem = memManager->allocate(sizeof(T));
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

inline std::size_t nextPowerOfTwo(std::size_t size) {
  if (size == 0) return 1;
  return std::pow(2, std::ceil(std::log2(size)));
}

template <typename T, typename... Args>
std::enable_if_t<std::is_default_constructible_v<T>, T*> gc_new_array(MemoryManager* memManager, size_t count,
                                                                      Args&&... args) {
  if (count == 0) return nullptr;

  std::size_t total_size = sizeof(T) * count;

  std::size_t aligned_size = nextPowerOfTwo(total_size);

  void* mem = memManager->allocate(aligned_size);
  if (!mem) return nullptr;

  T* array = static_cast<T*>(mem);
  for (std::size_t i = 0; i < count; ++i) {
    new (array + i) T(std::forward<Args>(args)...);
  }
  getArrayTable().emplace_back(ArrayInfo{array, count, sizeof(T)});

  return array;
}

}  // namespace memManagerNS