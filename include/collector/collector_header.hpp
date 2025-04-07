#pragma once

#include <unordered_set>

#include "Stack_scanner/StackScanner.hpp"

namespace GC {

class GarbageCollector {
 public:
  GarbageCollector() = default;

  void init(MemoryManager& memManager);

  void runGarbageCollector();

 private:
  MemoryManager* memManager_ = nullptr;
  StackScanner* stackScanner_ = nullptr;
};

extern GarbageCollector garbageCollector;

}  // namespace GC