#include "Base_ds/header_lists.hpp"

void* FreeList::allocate() {
  if (freeBlocks_.empty()) {
    return nullptr;
  }
  auto it = freeBlocks_.begin();
  void* block = *it;
  freeBlocks_.erase(it);
  return block;
}

void FreeList::deallocate(void* ptr) {
  assert(ptr && "Cannot deallocate a null pointer");
  if (freeBlocks_.find(ptr) != freeBlocks_.end()) {
    std::cerr << "Attempting to deallocate a pointer that is already in the free list." << std::endl;
    return;
  }
  freeBlocks_.insert(ptr);
}

bool FreeList::isEmpty() const { return freeBlocks_.empty(); }

void FreeList::addBlock(void* ptr) { deallocate(ptr); }

void FreeList::dump() const {
  std::cout << "FreeList state: ";
  for (auto ptr : freeBlocks_) {
    std::cout << ptr << " -> ";
  }
  std::cout << "nullptr" << std::endl;
}