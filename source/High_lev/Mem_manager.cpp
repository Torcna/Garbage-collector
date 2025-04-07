#include "High_lvl/Mem_manager.hpp"

MemoryManager memManager;

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
  chunks_.emplace_back(object_size);
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