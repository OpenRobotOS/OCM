#include <ocm/ocm.hpp>

#include <iostream>

int main() {
  // 示例使用 AtomicPtr
  openrobot::ocm::AtomicPtr<int> atomicPtr;

  atomicPtr = 42;
  auto value = atomicPtr();
  std::cout << "AtomicPtr value: " << *value << std::endl;

  // 示例使用 MutexPtr
  openrobot::ocm::MutexPtr<int> mutexPtr;
  mutexPtr.LockWrite();
  *mutexPtr.data_ptr_ = 100;
  mutexPtr.UnlockWrite();
  std::cout << "MutexPtr value: " << *mutexPtr.data_ptr_ << std::endl;

  //   // 示例使用 SharedMemoryObject
  //   openrobot::ocm::SharedMemoryObject<int> sharedMemory;
  //   sharedMemory.createNew("example-shared-memory", true);
  //   *sharedMemory() = 256;
  //   std::cout << "SharedMemoryObject value: " << *sharedMemory() << std::endl;

  return 0;
}
