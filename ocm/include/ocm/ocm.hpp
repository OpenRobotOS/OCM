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
#include <cstdint>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "common/ip_tool.hpp"

namespace openrobot::ocm {

/**
 * @brief Atomic pointer wrapper for thread-safe read and write operations.
 *
 * The `AtomicPtr` class provides a thread-safe way to manage a shared pointer to a constant
 * object of type `T`. It ensures that reads and writes to the pointer are atomic, preventing
 * data races in multi-threaded environments.
 *
 * @tparam T The type of the object to be pointed to.
 */
template <typename T>
class AtomicPtr {
 public:
  /**
   * @brief Default constructor.
   *
   * Initializes the atomic pointer with a default-constructed `std::shared_ptr<const T>`.
   */
  AtomicPtr() { data_ptr_.store(std::make_shared<const T>()); }

  /**
   * @brief Constructs an AtomicPtr with the given data.
   *
   * Initializes the atomic pointer with a `std::shared_ptr<const T>` pointing to a copy of `data`.
   *
   * @param data The data to be stored in the atomic pointer.
   */
  explicit AtomicPtr(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }

  /**
   * @brief Deleted copy constructor.
   *
   * Prevents copying of `AtomicPtr` instances to maintain unique ownership semantics.
   */
  AtomicPtr(const AtomicPtr&) { throw std::logic_error("[AtomicPtr] Data copy construction is not allowed!"); }

  /**
   * @brief Deleted copy assignment operator.
   *
   * Prevents assignment from one `AtomicPtr` to another to maintain unique ownership semantics.
   */
  AtomicPtr& operator=(const AtomicPtr&) = delete;

  /**
   * @brief Deleted move constructor.
   *
   * Prevents moving of `AtomicPtr` instances to maintain unique ownership semantics.
   */
  AtomicPtr(AtomicPtr&&) = delete;

  /**
   * @brief Deleted move assignment operator.
   *
   * Prevents move assignment of `AtomicPtr` instances to maintain unique ownership semantics.
   */
  AtomicPtr& operator=(AtomicPtr&&) = delete;

  /**
   * @brief Destructor.
   *
   * Default destructor ensures proper cleanup of the shared pointer.
   */
  ~AtomicPtr() = default;

  /**
   * @brief Assigns new data to the atomic pointer.
   *
   * Creates a new `std::shared_ptr<const T>` pointing to a copy of `data` and atomically stores it.
   *
   * @param data The new data to be stored.
   */
  void operator=(const T& data) { data_ptr_.store(std::make_shared<const T>(data), std::memory_order_release); }

  /**
   * @brief Retrieves the shared pointer.
   *
   * Atomically loads and returns the current `std::shared_ptr<const T>`.
   *
   * @return A shared pointer to the constant object of type `T`.
   */
  std::shared_ptr<const T> GetPtr() const { return data_ptr_.load(std::memory_order_acquire); }

  /**
   * @brief Retrieves the value pointed to by the shared pointer.
   *
   * Atomically loads the shared pointer and returns a copy of the object it points to.
   *
   * @return A copy of the object of type `T`.
   */
  T GetValue() const { return *data_ptr_.load(std::memory_order_acquire); }

 private:
  std::atomic<std::shared_ptr<const T>> data_ptr_; /**< Atomic shared pointer to a constant object of type `T` */
};

/**
 * @brief Read-Write lock protected data wrapper.
 *
 * The `RWLockData` class provides a thread-safe way to manage a shared pointer to an object of type `T`.
 * It allows multiple concurrent readers or exclusive writers, ensuring data integrity in multi-threaded
 * environments.
 *
 * @tparam T The type of the object to be managed.
 */
template <typename T>
class RWLockData {
 public:
  /**
   * @brief Default constructor.
   *
   * Initializes the shared pointer with a default-constructed object of type `T`.
   */
  RWLockData() { data_ptr_ = std::make_shared<T>(); }

  /**
   * @brief Constructs RWLockData with the given data.
   *
   * Initializes the shared pointer with a copy of `data`.
   *
   * @param data The data to be stored.
   */
  explicit RWLockData(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief Deleted copy constructor.
   *
   * Prevents copying of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData(const RWLockData&) { throw std::logic_error("[RWLockData] Data copy construction is not allowed!"); }

  /**
   * @brief Deleted copy assignment operator.
   *
   * Prevents assignment from one `RWLockData` to another to maintain unique ownership semantics.
   */
  RWLockData& operator=(const RWLockData&) = delete;

  /**
   * @brief Deleted move constructor.
   *
   * Prevents moving of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData(RWLockData&&) = delete;

  /**
   * @brief Deleted move assignment operator.
   *
   * Prevents move assignment of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData& operator=(RWLockData&&) = delete;

  /**
   * @brief Destructor.
   *
   * Default destructor ensures proper cleanup of the shared pointer.
   */
  ~RWLockData() = default;

  /**
   * @brief Acquires a shared (read) lock.
   *
   * Allows multiple threads to read the data concurrently.
   */
  void LockRead() { mutex_.lock_shared(); }

  /**
   * @brief Attempts to acquire a shared (read) lock without blocking.
   *
   * @return `true` if the lock was successfully acquired; otherwise, `false`.
   */
  bool TryLockRead() { return mutex_.try_lock_shared(); }

  /**
   * @brief Releases a shared (read) lock.
   */
  void UnlockRead() { mutex_.unlock_shared(); }

  /**
   * @brief Acquires an exclusive (write) lock.
   *
   * Ensures exclusive access to the data, blocking other readers and writers.
   */
  void LockWrite() { mutex_.lock(); }

  /**
   * @brief Attempts to acquire an exclusive (write) lock without blocking.
   *
   * @return `true` if the lock was successfully acquired; otherwise, `false`.
   */
  bool TryLockWrite() { return mutex_.try_lock(); }

  /**
   * @brief Releases an exclusive (write) lock.
   */
  void UnlockWrite() { mutex_.unlock(); }

  /**
   * @brief Assigns new data to the shared pointer.
   *
   * Creates a new `std::shared_ptr<T>` pointing to a copy of `data` and assigns it atomically.
   *
   * @param data The new data to be stored.
   */
  void operator=(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief Retrieves the shared pointer.
   *
   * @return A shared pointer to the object of type `T`.
   */
  std::shared_ptr<T> GetPtr() { return data_ptr_; }

  /**
   * @brief Retrieves the value pointed to by the shared pointer.
   *
   * @return A copy of the object of type `T`.
   */
  T GetValue() { return *data_ptr_; }

 private:
  std::shared_ptr<T> data_ptr_; /**< Shared pointer to the managed object of type `T` */
  std::shared_mutex mutex_;     /**< Shared mutex for managing read-write locks */
};

/**
 * @brief Semaphore wrapper for managing inter-process synchronization.
 *
 * The `SharedMemorySemaphore` class provides an interface to named POSIX semaphores,
 * allowing multiple processes to synchronize access to shared resources.
 */
class SharedMemorySemaphore {
 public:
  /**
   * @brief Constructs a SharedMemorySemaphore with the given name and initial value.
   *
   * Initializes the semaphore by calling the `Init` method.
   *
   * @param name The name identifier for the semaphore.
   * @param value The initial value of the semaphore.
   */
  SharedMemorySemaphore(const std::string& name, unsigned int value);

  /**
   * @brief Destructor.
   *
   * Default destructor ensures proper cleanup of the semaphore.
   */
  ~SharedMemorySemaphore() = default;

  /**
   * @brief Initializes the semaphore.
   *
   * Opens an existing named semaphore or creates a new one if it does not exist.
   *
   * @param sem_name The name identifier for the semaphore.
   * @param value The initial value of the semaphore.
   *
   * @throws std::runtime_error If semaphore initialization fails.
   */
  void Init(const std::string& sem_name, unsigned int value);

  /**
   * @brief Increments the semaphore's value.
   *
   * Equivalent to a "post" operation, increasing the semaphore's count by one.
   *
   * @throws std::runtime_error If the increment operation fails.
   */
  void Increment();

  /**
   * @brief Increments the semaphore's value if it is currently zero.
   *
   * Checks the semaphore's current value and only increments it if it is zero.
   *
   * @throws std::runtime_error If the increment operation fails.
   */
  void IncrementWhenZero();

  /**
   * @brief Increments the semaphore's value by a specified amount.
   *
   * Performs multiple "post" operations to increase the semaphore's count.
   *
   * @param value The amount by which to increment the semaphore.
   *
   * @throws std::runtime_error If any increment operation fails.
   */
  void Increment(unsigned int value);

  /**
   * @brief Decrements the semaphore's value.
   *
   * Equivalent to a "wait" operation, decreasing the semaphore's count by one.
   *
   * @throws std::runtime_error If the decrement operation fails.
   */
  void Decrement();

  /**
   * @brief Attempts to decrement the semaphore's value without blocking.
   *
   * Tries to perform a "wait" operation. If the semaphore's value is greater than zero,
   * it decrements the value and returns `true`. Otherwise, it returns `false`.
   *
   * @return `true` if the semaphore was successfully decremented; otherwise, `false`.
   */
  bool TryDecrement();

  /**
   * @brief Attempts to decrement the semaphore's value with a timeout.
   *
   * Waits for the semaphore to become available until the specified timeout.
   *
   * @param seconds The number of seconds to wait.
   * @param nanoseconds The number of nanoseconds to wait.
   * @return `true` if the semaphore was successfully decremented within the timeout; otherwise, `false`.
   *
   * @throws std::runtime_error If retrieving the current time fails.
   */
  bool DecrementTimeout(uint64_t seconds, uint64_t nanoseconds);

  /**
   * @brief Retrieves the current value of the semaphore.
   *
   * @return The current count of the semaphore.
   *
   * @throws std::runtime_error If retrieving the semaphore's value fails.
   */
  int GetValue() const;

  /**
   * @brief Destroys the semaphore.
   *
   * Closes and unlinks the semaphore, removing it from the system.
   *
   * @throws std::runtime_error If closing or unlinking the semaphore fails.
   */
  void Destroy();

 private:
  sem_t* sem_ = nullptr; /**< Pointer to the POSIX semaphore */
  std::string name_;     /**< Name identifier for the semaphore */
};

template <typename T>
class SharedMemoryData {
 public:
  SharedMemoryData(const std::string& name, bool check_size, size_t size = 0) : sem_(name + "_shm", 1), data_(nullptr), fd_(0) {
    Init(name, check_size, size);
  }
  ~SharedMemoryData() = default;

  void Init(const std::string& name, bool check_size, size_t size) {
    assert(!data_);
    bool is_create = false;
    name_ = "openrobot_ocm_" + name;
    size_ = size;

    fd_ = shm_open(name_.c_str(), O_RDWR, 0);
    if (fd_ == -1) {
      if (errno == ENOENT) {
        fd_ = shm_open(name_.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
        if (fd_ == -1) {
          throw std::runtime_error("[SharedMemoryData] Failed to create shared memory \"" + name_ + "\": " + std::string(strerror(errno)));
        }
        if (ftruncate(fd_, size_) != 0) {
          throw std::runtime_error("[SharedMemoryData] ftruncate failed for \"" + name_ + "\": " + std::string(strerror(errno)));
        }
        is_create = true;
      } else {
        throw std::runtime_error("[SharedMemoryData] shm_open failed for \"" + name_ + "\": " + std::string(strerror(errno)));
      }
    } else {
      struct stat s;
      if (fstat(fd_, &s)) {
        throw std::runtime_error("[SharedMemoryData] fstat failed for \"" + name_ + "\": " + std::string(strerror(errno)));
      }
      if (check_size) {
        if ((size_t)s.st_size != size_) {
          throw std::runtime_error("[SharedMemoryData] Existing shared memory \"" + name_ + "\" size mismatch! Expected: " + std::to_string(size_) +
                                   ", Actual: " + std::to_string(s.st_size));
        }
      } else {
        size_ = s.st_size;
      }
    }

    void* mem = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (mem == MAP_FAILED) {
      throw std::runtime_error("[SharedMemoryData] mmap failed for \"" + name_ + "\": " + std::string(strerror(errno)));
    }
    if (is_create) {
      memset(mem, 0, size_);
    }
    data_ = static_cast<T*>(mem);
  }

  void CloseExisting() {
    sem_.Destroy();
    assert(data_);
    if (munmap(static_cast<void*>(data_), size_) != 0) {
      throw std::runtime_error("[SharedMemoryData::CloseExisting] munmap failed: " + std::string(strerror(errno)));
    }
    data_ = nullptr;
    if (shm_unlink(name_.c_str()) != 0) {
      if (errno != ENOENT) {
        throw std::runtime_error("[SharedMemoryData::CloseExisting] shm_unlink failed: " + std::string(strerror(errno)));
      }
    }
    if (close(fd_) != 0) {
      throw std::runtime_error("[SharedMemoryData::CloseExisting] close failed: " + std::string(strerror(errno)));
    }
    fd_ = 0;
  }
  void Detach() {
    assert(data_);
    if (munmap(static_cast<void*>(data_), size_) != 0) {
      throw std::runtime_error("[SharedMemoryData::Detach] munmap failed: " + std::string(strerror(errno)));
    }
    data_ = nullptr;
    if (close(fd_) != 0) {
      throw std::runtime_error("[SharedMemoryData::Detach] close failed: " + std::string(strerror(errno)));
    }
    fd_ = 0;
  }
  T* Get() {
    assert(data_);
    return data_;
  }
  void Lock() { sem_.Decrement(); }
  void UnLock() { sem_.Increment(); }
  int GetSize() const { return static_cast<int>(size_); }

 private:
  SharedMemorySemaphore sem_;
  T* data_ = nullptr;
  std::string name_;
  size_t size_;
  int fd_;
};

class SharedMemoryTopic {
 public:
  SharedMemoryTopic() = default;
  SharedMemoryTopic(const SharedMemoryTopic&) = delete;
  SharedMemoryTopic& operator=(const SharedMemoryTopic&) = delete;
  SharedMemoryTopic(SharedMemoryTopic&&) = delete;
  SharedMemoryTopic& operator=(SharedMemoryTopic&&) = delete;
  ~SharedMemoryTopic() {
    for (auto& shm : shm_map_) {
      shm.second->CloseExisting();
    }
  }

  template <class MessageType>
  void Publish(const std::string& topic_name, const std::string& shm_name, const MessageType* msg) {
    WriteDataToSHM(shm_name, msg);
    PublishSem(topic_name);
  }

  template <class MessageType>
  void PublishList(const std::vector<std::string>& topic_name, const std::string& shm_name, const std::vector<MessageType>& msgs) {
    WriteDataToSHM(shm_name, msgs);
    for (const auto& topic : topic_name) {
      PublishSem(topic);
    }
  }

  template <class MessageType, typename Callback>
  void Subscribe(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->Decrement();
    CheckSHMExist(shm_name, false);
    MessageType msg;
    shm_map_.at(shm_name)->Lock();
    msg.decode(shm_map_.at(shm_name)->Get(), 0, shm_map_.at(shm_name)->GetSize());
    shm_map_.at(shm_name)->UnLock();
    callback(msg);
  }

  template <class MessageType, typename Callback>
  void SubscribeNoWait(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    if (sem_map_.at(topic_name)->TryDecrement()) {
      CheckSHMExist(shm_name, false);
      MessageType msg;
      shm_map_.at(shm_name)->Lock();
      msg.decode(shm_map_.at(shm_name)->Get(), 0, shm_map_.at(shm_name)->GetSize());
      shm_map_.at(shm_name)->UnLock();
      callback(msg);
    }
  }

 private:
  template <class MessageType>
  void WriteDataToSHM(const std::string& shm_name, const MessageType* msg) {
    int datalen = msg->getEncodedSize();
    CheckSHMExist(shm_name, true, datalen);
    shm_map_.at(shm_name)->Lock();
    msg->encode(shm_map_.at(shm_name)->Get(), 0, datalen);
    shm_map_.at(shm_name)->UnLock();
  }
  void PublishSem(const std::string& topic_name) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->IncrementWhenZero();
  }
  void CheckSHMExist(const std::string& shm_name, bool check_size, int size = 0) {
    if (shm_map_.find(shm_name) == shm_map_.end()) {
      shm_map_.emplace(shm_name, std::make_shared<SharedMemoryData<uint8_t>>(shm_name, check_size, size));
    }
  }
  void CheckSemExist(const std::string& topic_name) {
    if (sem_map_.find(topic_name) == sem_map_.end()) {
      sem_map_.emplace(topic_name, std::make_shared<SharedMemorySemaphore>(topic_name, 0));
    }
  }
  std::unordered_map<std::string, std::shared_ptr<SharedMemoryData<uint8_t>>> shm_map_;
  std::unordered_map<std::string, std::shared_ptr<SharedMemorySemaphore>> sem_map_;
};

}  // namespace openrobot::ocm