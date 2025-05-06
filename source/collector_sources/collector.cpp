#include <pthread.h>

#include <cstdint>
#include <iostream>
#include <unordered_set>

#include "High_lvl/Mem_manager.hpp"
#include "collector/collector_header.hpp"

namespace GC {

GarbageCollector garbageCollector;

void GarbageCollector::init(MemoryManager* memManager) { stackScanner_ = new StackScanner(memManager); }
void GarbageCollector::runGarbageCollector() {
  if (!stackScanner_) {
    std::cerr << "GarbageCollector is not initialized!" << std::endl;
    return;
  }

  stackScanner_->scanStack();

  const auto& rootSet = stackScanner_->getRootSet();
  for (void* root : rootSet) {
    std::cout << "Root object: " << root << std::endl;
  }
  for (void* root : rootSet) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(root);
    uintptr_t chunk_addr = addr & ~uintptr_t(0xFFF);
    Chunk* chunk = reinterpret_cast<Chunk*>(chunk_addr);

    std::cout << "Chunk address (sliced): " << (void*)chunk_addr << std::endl;
    std::cout << "Chunk memory_ (real):   " << static_cast<void*>(chunk->getStart()) << std::endl;

    uint8_t* dataStart = chunk->getData();
    size_t objSize = chunk->getObjectSize();

    if (root >= dataStart && root < dataStart + objSize * chunk->getNumObjects()) {
      size_t index = (static_cast<uint8_t*>(root) - dataStart) / objSize;
      chunk->markObject(index);
    }
  }
}

}  // namespace GC