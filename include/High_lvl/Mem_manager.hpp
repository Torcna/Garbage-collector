#pragma once
#include <unordered_map>
#include <vector>

#include "Base_ds/header_chunk.hpp"
#include "Base_ds/header_lists.hpp"

class MemoryManager {
 private:
  uint8_t* arena_ = nullptr;
  size_t arenaSize_ = 0;
  size_t arenaOffset_ = 0;
  std::unordered_map<size_t, FreeList> free_lists_;
  std::vector<Chunk> chunks_;

 public:
  MemoryManager(size_t size);
  void initArena(size_t size);
  void* allocFromArena(size_t size, size_t align = 4096);
  void* allocate(size_t size);
  void deallocate(void* ptr, size_t size);
  void addChunk(size_t object_size);
  bool isInMyHeap(void* ptr);
  void sweepUnmarkedObjects();
};