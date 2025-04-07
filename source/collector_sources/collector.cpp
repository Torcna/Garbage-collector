#include <pthread.h>

#include <cstdint>
#include <iostream>
#include <unordered_set>

#include "High_lvl/Mem_manager.hpp"
#include "collector/collector_header.hpp"

namespace GC {

GarbageCollector garbageCollector;

void GarbageCollector::init(MemoryManager& memManager) {
  memManager_ = &memManager;
  stackScanner_ = new StackScanner(memManager);
}
void GarbageCollector::runGarbageCollector() {
  if (!memManager_ || !stackScanner_) {
    std::cerr << "GarbageCollector is not initialized!" << std::endl;
    return;
  }

  stackScanner_->scanStack();

  const auto& rootSet = stackScanner_->getRootSet();
}

}  // namespace GC