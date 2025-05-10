#include "High_lvl/Mem_manager.hpp"

#include <algorithm>
#include <cstdlib>

MemoryManager::MemoryManager(size_t size) : arenaSize_(size), arenaOffset_(0) {
  // Initialize the arena with a default size
  initArena(size);
}

void* MemoryManager::allocate(size_t size) {
  auto it = free_lists_.find(size);
  if (it != free_lists_.end()) {
    void* block = it->second.allocate();
    if (block) return block;
  }

  addChunk(size);
  return free_lists_[size].allocate();
}

void MemoryManager::deallocate(void* ptr, size_t size) { free_lists_[size].deallocate(ptr); }

void MemoryManager::addChunk(size_t object_size) {
  chunks_.emplace_back(this, object_size);
  Chunk& chunk = chunks_.back();

  FreeList& free_list = free_lists_[object_size];

  for (size_t i = 0, sz = chunk.getNumObjects(); i < sz; ++i) {
    free_list.addBlock(chunk.getObject(i));
  }
}

bool MemoryManager::isInMyHeap(void* ptr) {
  if (chunks_.empty()) {
    return false;
  }

  void* heapStart = chunks_.front().getStart();
  void* heapEnd = static_cast<char*>(chunks_.back().getStart()) + chunks_.back().getSize();

  return ptr >= heapStart && ptr < heapEnd;
}

void MemoryManager::initArena(size_t size) {
  arena_ = static_cast<uint8_t*>(std::aligned_alloc(4096, size));
  arenaSize_ = size;
  arenaOffset_ = 0;
}

void* MemoryManager::allocFromArena(size_t size, size_t align) {
  size_t current = reinterpret_cast<size_t>(arena_) + arenaOffset_;
  size_t aligned = (current + align - 1) & ~(align - 1);
  size_t offset = aligned - reinterpret_cast<size_t>(arena_);
  if (offset + size > arenaSize_) return nullptr;
  arenaOffset_ = offset + size;
  return arena_ + offset;
}

void MemoryManager::sweepUnmarkedObjects() {
  for (auto& chunk : chunks_) {
    size_t numObjects = chunk.getNumObjects();
    size_t objSize = chunk.getObjectSize();
    uint8_t* data = chunk.getData();

    for (size_t i = 0; i < numObjects; ++i) {
      if (!chunk.isObjectMarked(i)) {
        deallocate(data + i * objSize, objSize);
      }
    }

    chunk.resetMarks();
  }
}