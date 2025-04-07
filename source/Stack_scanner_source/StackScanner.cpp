#include "Stack_scanner/StackScanner.hpp"

#include <pthread.h>

#include "High_lvl/Mem_manager.hpp"

StackScanner::StackScanner(MemoryManager& memManager) : memManager_(memManager) {}

void* StackScanner::getStackTop() {
  volatile int stackVar = 0;
  return (void*)&stackVar;
}

void* StackScanner::getStackBottom() {
  pthread_attr_t attr;
  if (pthread_getattr_np(pthread_self(), &attr) != 0) {
    return nullptr;
  }

  void* stackAddr = nullptr;
  size_t stackSize = 0;
  if (pthread_attr_getstack(&attr, &stackAddr, &stackSize) != 0) {
    pthread_attr_destroy(&attr);
    return nullptr;
  }
  pthread_attr_destroy(&attr);

  return static_cast<char*>(stackAddr) + stackSize;
}

bool StackScanner::isPointerToHeap(void* ptr) { return memManager_.isInMyHeap(ptr); }

void StackScanner::scanStack() {
  rootSet.clear();

  void* stackTop = getStackTop();
  void* stackBottom = getStackBottom();

  if (stackTop > stackBottom) {
    for (void** addr = static_cast<void**>(stackTop); addr >= static_cast<void**>(stackBottom); --addr) {
      void* potentialPtr = *addr;
      if (isPointerToHeap(potentialPtr)) {
        rootSet.insert(potentialPtr);
      }
    }
  } else {
    for (void** addr = static_cast<void**>(stackBottom); addr >= static_cast<void**>(stackTop); --addr) {
      void* potentialPtr = *addr;
      if (isPointerToHeap(potentialPtr)) {
        rootSet.insert(potentialPtr);
      }
    }
  }
}

const std::unordered_set<void*>& StackScanner::getRootSet() const { return rootSet; }