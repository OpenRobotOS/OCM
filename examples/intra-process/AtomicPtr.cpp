#include <iostream>
#include <ocm/ocm.hpp>
#include <thread>

// Example function that modifies data using AtomicPtr
void UpdateData(openrobot::ocm::AtomicPtr<int>& atomicPtr) {
  // Modify the data in a thread-safe manner
  atomicPtr = 10;
  std::shared_ptr<const int> value = atomicPtr();
  std::cout << "Updated value: " << *value << std::endl;
}

int main() {
  openrobot::ocm::AtomicPtr<int> atomicInt(5);  // Initialize with value 5

  // Use multiple threads to modify and read the atomic data
  std::thread t1(UpdateData, std::ref(atomicInt));
  std::thread t2(UpdateData, std::ref(atomicInt));

  t1.join();
  t2.join();

  return 0;
}
