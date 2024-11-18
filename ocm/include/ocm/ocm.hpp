#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>  // for size_t
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>

namespace openrobot::ocm {

enum TopicType { SHM = 0, LCM = 1 };

/*!
 * intra-process
 */

template <typename T>
class AtomicPtr {
 public:
  AtomicPtr() { data_ptr_.store(std::make_shared<const T>()); }
  explicit AtomicPtr(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }
  AtomicPtr(const AtomicPtr&) { throw std::logic_error("[AtomicPtr]data copy construct!"); }
  AtomicPtr& operator=(const AtomicPtr&) = delete;
  AtomicPtr(AtomicPtr&&) = delete;
  AtomicPtr& operator=(AtomicPtr&&) = delete;
  ~AtomicPtr() = default;

  void operator=(const T& data) { data_ptr_.store(std::make_shared<const T>(data), std::memory_order_release); }
  std::shared_ptr<const T> operator()() const { return data_ptr_.load(std::memory_order_acquire); }

 private:
  std::atomic<std::shared_ptr<const T>> data_ptr_;
};

template <typename T>
class RWLockData {
 public:
  RWLockData() { data_ptr_ = std::make_shared<T>(); }
  explicit RWLockData(const T& data) { data_ptr_ = std::make_shared<T>(data); }
  RWLockData(const RWLockData&) { throw std::logic_error("[RWLockData]data copy construct!"); }
  RWLockData& operator=(const RWLockData&) = delete;
  RWLockData(RWLockData&&) = delete;
  RWLockData& operator=(RWLockData&&) = delete;
  ~RWLockData() = default;

  void LockRead() { mutex_.lock_shared(); }
  bool TryLockRead() { return mutex_.try_lock_shared(); }
  void UnlockRead() { mutex_.unlock_shared(); }
  void LockWrite() { mutex_.lock(); }
  bool TryLockWrite() { return mutex_.try_lock(); }
  void UnlockWrite() { mutex_.unlock(); }
  T& Get() {
    assert(data_ptr_);
    return data_ptr_;
  }

 private:
  std::shared_ptr<T> data_ptr_;
  std::shared_mutex mutex_;
};

/*!
 * inter-process
 */

/*!
 * shared memory
 */

class SharedMemorySemaphore {
 public:
  SharedMemorySemaphore(const std::string& name, unsigned int value);
  SharedMemorySemaphore(const SharedMemorySemaphore&);
  SharedMemorySemaphore& operator=(const SharedMemorySemaphore&) = delete;
  SharedMemorySemaphore(SharedMemorySemaphore&&) = delete;
  SharedMemorySemaphore& operator=(SharedMemorySemaphore&&) = delete;
  ~SharedMemorySemaphore() = default;

  void Init(const std::string& sem_name, unsigned int value);
  void Increment();
  void Increment(unsigned int value);
  void Decrement();
  bool TryDecrement();
  bool DecrementTimeout(uint64_t seconds, uint64_t nanoseconds);
  int GetValue() const;
  void Destroy();

 private:
  sem_t* sem_ = nullptr;  // 信号量
  std::string name_;      // 信号量的名字
};

class SharedMemoryData {
 public:
  SharedMemoryData(const std::string& name, size_t size);
  SharedMemoryData(const SharedMemoryData&);
  SharedMemoryData& operator=(const SharedMemoryData&) = delete;
  SharedMemoryData(SharedMemoryData&&) = delete;
  SharedMemoryData& operator=(SharedMemoryData&&) = delete;
  ~SharedMemoryData() = default;

  void Init(const std::string& name, size_t size);

  void CloseExisting();

  void Detach();

  uint8_t* Get();
  void Lock();
  void UnLock();

 private:
  SharedMemorySemaphore sem_;
  uint8_t* data_ = nullptr;
  std::string name_;
  size_t size_;
  int fd_;
};

template <typename T>
class TopicHandler {
 public:
  TopicHandler() = default;
  explicit TopicHandler(const T& data) : data_(data) {}
  TopicHandler(const TopicHandler&) { throw std::logic_error("[TopicHandler]copy construct!"); }
  TopicHandler& operator=(const TopicHandler&) = delete;
  TopicHandler(TopicHandler&&) = delete;
  TopicHandler& operator=(TopicHandler&&) = delete;
  ~TopicHandler() = default;

  void LockRead() { mutex_.lock_shared(); }
  bool TryLockRead() { return mutex_.try_lock_shared(); }
  void UnlockRead() { mutex_.unlock_shared(); }
  void LockWrite() { mutex_.lock(); }
  bool TryLockWrite() { return mutex_.try_lock(); }
  void UnlockWrite() { mutex_.unlock(); }
  T data_;

 private:
  std::shared_mutex mutex_;
};

}  // namespace openrobot::ocm