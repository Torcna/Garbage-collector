#pragma once

#include <unordered_set>

#include "Stack_scanner/StackScanner.hpp"

namespace GC {

class GarbageCollector {
 public:
  void init(MemoryManager* memManager);
  void runGarbageCollector();

 private:
  void sweepUnmarkedObjects(MemoryManager* memManager);
  StackScanner* stackScanner_ = nullptr;
};

extern GarbageCollector garbageCollector;

}  // namespace GC