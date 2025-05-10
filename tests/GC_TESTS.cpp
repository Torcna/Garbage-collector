#include <gtest/gtest.h>

#include "High_lvl/GC_B.hpp"
#include "collector/collector_header.hpp"

struct Dummy {
  int x;
  Dummy(int v = 0) : x(v) {}
};

TEST(MemoryManagerTest, AllocateAndDeallocate) {
  MemoryManager memManager(4096 * 4);
  memManager.initArena(4096 * 4);

  void* ptr1 = memManager.allocate(sizeof(Dummy));
  ASSERT_NE(ptr1, nullptr);

  void* ptr2 = memManager.allocate(sizeof(Dummy));
  ASSERT_NE(ptr2, nullptr);

  memManager.deallocate(ptr1, sizeof(Dummy));
  memManager.deallocate(ptr2, sizeof(Dummy));
}

TEST(MemoryManagerTest, ChunkExpansionAndGC) {
  MemoryManager memManager(16 * 1024 * 1024);
  GC::garbageCollector.init(&memManager);

  constexpr size_t N = 2000;
  std::vector<Dummy*> ptrs;
  ptrs.reserve(N);

  for (size_t i = 0; i < N; ++i) {
    auto* obj = memManagerNS::gc_new<Dummy>(&memManager, int(i));
    ASSERT_NE(obj, nullptr);
    ptrs.push_back(obj);
  }

  for (size_t i = 0; i < N; ++i) {
    ASSERT_EQ(ptrs[i]->x, int(i));
  }

  GC::garbageCollector.runGarbageCollector();
}