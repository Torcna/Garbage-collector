#pragma once

#include <cstddef>
#include <unordered_set>

#include "High_lvl/Mem_manager.hpp"

class StackScanner {
 public:
  explicit StackScanner(MemoryManager& memManager);

  void* getStackTop();
  void* getStackBottom();

  void scanStack();

  const std::unordered_set<void*>& getRootSet() const;

 private:
  MemoryManager& memManager_;
  std::unordered_set<void*> rootSet;

  bool isPointerToHeap(void* ptr);
};