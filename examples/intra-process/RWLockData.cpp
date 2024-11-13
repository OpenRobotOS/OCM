#include <iostream>
#include <ocm/ocm.hpp>
#include <thread>

// Example function to read data using a read lock
void ReadData(openrobot::ocm::RWLockData<int>& rwData) {
  rwData.LockRead();  // Acquire read lock
  std::cout << "Read value: " << rwData.data_ << std::endl;
  rwData.UnlockRead();  // Release read lock
}

// Example function to modify data using a write lock
void WriteData(openrobot::ocm::RWLockData<int>& rwData, int newValue) {
  rwData.LockWrite();  // Acquire write lock
  rwData.data_ = newValue;
  std::cout << "Updated value to: " << rwData.data_ << std::endl;
  rwData.UnlockWrite();  // Release write lock
}

int main() {
  openrobot::ocm::RWLockData<int> rwData(10);  // Initialize with value 10

  // Use multiple threads to read and write to shared data
  std::thread t1(ReadData, std::ref(rwData));
  std::thread t2(WriteData, std::ref(rwData), 20);
  std::thread t3(ReadData, std::ref(rwData));

  t1.join();
  t2.join();
  t3.join();

  return 0;
}