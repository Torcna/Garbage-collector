#include <cstdint>
#include <iostream>
#include <unordered_set>

#include "High_lvl/Mem_manager.hpp"
#include "collector/collector_header.hpp"
namespace {
struct ChunkHeader {
  size_t object_size_;
  size_t num_objects_;
  void* next_;
};

inline ChunkHeader* getChunkHeader(void* chunk_addr) { return reinterpret_cast<ChunkHeader*>(chunk_addr); }

inline uint8_t* getMarkBits(ChunkHeader* header) {
  size_t header_size = sizeof(ChunkHeader);
  return reinterpret_cast<uint8_t*>(header) + header_size;
}

inline uint8_t* getData(ChunkHeader* header) {
  size_t header_size = sizeof(ChunkHeader);
  size_t mark_bits_size = (header->num_objects_ + 7) / 8;
  return reinterpret_cast<uint8_t*>(header) + header_size + mark_bits_size;
}

inline bool isPointerInChunk(void* ptr, ChunkHeader* header) {
  uint8_t* data = getData(header);
  size_t obj_size = header->object_size_;
  size_t num_objs = header->num_objects_;
  return ptr >= data && ptr < data + obj_size * num_objs;
}

inline size_t getObjectIndex(void* ptr, ChunkHeader* header) {
  uint8_t* data = getData(header);
  size_t obj_size = header->object_size_;
  return (static_cast<uint8_t*>(ptr) - data) / obj_size;
}

inline void markObject(ChunkHeader* header, size_t index) {
  uint8_t* mark_bits = getMarkBits(header);
  mark_bits[index / 8] |= (1 << (index % 8));
}

}  // namespace

namespace GC {

GarbageCollector garbageCollector;

void GarbageCollector::sweepUnmarkedObjects(MemoryManager* memManager) { memManager->sweepUnmarkedObjects(); }

void GarbageCollector::init(MemoryManager* memManager) { stackScanner_ = new StackScanner(memManager); }
void GarbageCollector::runGarbageCollector() {
  if (!stackScanner_) {
    std::cerr << "GarbageCollector is not initialized!" << std::endl;
    return;
  }

  stackScanner_->scanStack();

  const auto& rootSet = stackScanner_->getRootSet();
  for (void* root : rootSet) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(root);
    uintptr_t chunk_addr = addr & ~uintptr_t(0xFFF);
    ChunkHeader* header = getChunkHeader(reinterpret_cast<void*>(chunk_addr));

    size_t index = getObjectIndex(root, header);
    std::cout << "[GC] Root: " << root << " | Chunk: " << reinterpret_cast<void*>(chunk_addr)
              << " | Data: " << static_cast<void*>(getData(header)) << " | Index: " << index
              << " | ObjSize: " << header->object_size_ << " | NumObjs: " << header->num_objects_ << std::endl;

    markObject(header, index);
  }

  sweepUnmarkedObjects(stackScanner_->getMemoryManager());
}

}  // namespace GC
