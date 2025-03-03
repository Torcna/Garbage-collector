#include <cstddef>
#include <iostream>
#include <cassert>
#include <set>

class FreeList {
 private:
  std::set<void*> freeBlocks_;

 public:
  FreeList() = default;

  void* allocate();

  void deallocate(void* ptr);

  bool isEmpty() const;

  // addBlock ןנמסעמ גûחûגאוע deallocate().
  void addBlock(void* ptr);

  void dump() const;
};