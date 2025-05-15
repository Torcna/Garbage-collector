#include "Stack_scanner/StackScanner.hpp"

#include "High_lvl/Mem_manager.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

StackScanner::StackScanner(MemoryManager* memManager) : memManager_(memManager) {}

void* StackScanner::getStackTop() {
  void* stackTop = nullptr;
  {
    volatile int stackVar = 0;
    stackTop = (void*)&stackVar;
  }
  return stackTop;
}

void* StackScanner::getStackBottom() {
#ifdef _WIN32
  ULONG_PTR lowLimit = 0, highLimit = 0;
  GetCurrentThreadStackLimits(&lowLimit, &highLimit);
  return (void*)highLimit;
#else
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
#endif
}

bool StackScanner::isPointerToHeap(void* ptr) { return memManager_->isInMyHeap(ptr); }

void StackScanner::scanStack() {
  rootSet.clear();

  void* stackTop = getStackTop();
  void* stackBottom = getStackBottom();

  if (stackTop > stackBottom) {
    for (void** addr = static_cast<void**>(stackTop); addr >= static_cast<void**>(stackBottom); --addr) {
      if (addr < static_cast<void**>(stackTop) || addr > static_cast<void**>(stackBottom)) {
        break;
      }
      void* potentialPtr = *addr;
      if (isPointerToHeap(potentialPtr)) {
        rootSet.insert(potentialPtr);
      }
    }
  } else {
    for (void** addr = static_cast<void**>(stackBottom); addr >= static_cast<void**>(stackTop); --addr) {
      auto* potentialPtr = *addr;
      if (isPointerToHeap(potentialPtr)) {
        rootSet.insert(potentialPtr);
      }
    }
  }
}

const std::unordered_set<void*>& StackScanner::getRootSet() const { return rootSet; }