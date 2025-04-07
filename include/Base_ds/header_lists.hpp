#pragma once
#include <cassert>
#include <cstddef>
#include <iostream>
#include <set>

class FreeList {
 private:
  std::set<void*> freeBlocks_;

 public:
  FreeList() = default;

  void* allocate();

  void deallocate(void* ptr);

  bool isEmpty() const;

  void addBlock(void* ptr);

  void dump() const;
};