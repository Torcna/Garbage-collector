#pragma once
#include <unordered_map>
#include <vector>

#include "Base_ds/header_chunk.hpp"
#include "Base_ds/header_lists.hpp"

class MemoryManager {
 private:
  std::unordered_map<size_t, FreeList> free_lists_;
  std::vector<Chunk> chunks_;

 public:
  MemoryManager() = default;

  void* allocate(size_t size);
  void deallocate(void* ptr, size_t size);
  void addChunk(size_t object_size);
  bool isInMyHeap(void* ptr);
};