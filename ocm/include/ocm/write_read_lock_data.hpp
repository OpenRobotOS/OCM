
#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <memory>
#include <shared_mutex>
#include <stdexcept>

namespace openrobot::ocm {
/**
 * @brief 读写锁保护的数据包装器。
 * @brief Read-Write lock protected data wrapper.
 *
 * `RWLockData` 类提供了一种线程安全的方式来管理指向类型 `T` 对象的共享指针。
 * The `RWLockData` class provides a thread-safe way to manage a shared pointer to an object of type `T`.
 * 它允许多个并发的读者或独占的写作者，确保多线程环境中的数据完整性。
 * It allows multiple concurrent readers or exclusive writers, ensuring data integrity in multi-threaded
 * environments.
 *
 * @tparam T 要管理的对象的类型。
 * @tparam T The type of the object to be managed.
 */
template <typename T>
class RWLockData {
 public:
  /**
   * @brief 默认构造函数。
   * @brief Default constructor.
   *
   * 使用默认构造的 `T` 类型对象初始化共享指针。
   * Initializes the shared pointer with a default-constructed object of type `T`.
   */
  RWLockData() { data_ptr_ = std::make_shared<T>(); }

  /**
   * @brief 使用给定数据构造 RWLockData。
   * @brief Constructs RWLockData with the given data.
   *
   * 使用 `data` 的副本初始化共享指针。
   * Initializes the shared pointer with a copy of `data`.
   *
   * @param data 要存储的数据。
   * @param data The data to be stored.
   */
  explicit RWLockData(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief 删除的拷贝构造函数。
   * @brief Deleted copy constructor.
   *
   * 防止复制 `RWLockData` 实例以保持唯一所有权语义。
   * Prevents copying of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData(const RWLockData&) { throw std::logic_error("[RWLockData] Data copy construction is not allowed!"); }

  /**
   * @brief 删除的拷贝赋值运算符。
   * @brief Deleted copy assignment operator.
   *
   * 防止将一个 `RWLockData` 赋值给另一个以保持唯一所有权语义。
   * Prevents assignment from one `RWLockData` to another to maintain unique ownership semantics.
   */
  RWLockData& operator=(const RWLockData&) = delete;

  /**
   * @brief 删除的移动构造函数。
   * @brief Deleted move constructor.
   *
   * 防止移动 `RWLockData` 实例以保持唯一所有权语义。
   * Prevents moving of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData(RWLockData&&) = delete;

  /**
   * @brief 删除的移动赋值运算符。
   * @brief Deleted move assignment operator.
   *
   * 防止移动赋值 `RWLockData` 实例以保持唯一所有权语义。
   * Prevents move assignment of `RWLockData` instances to maintain unique ownership semantics.
   */
  RWLockData& operator=(RWLockData&&) = delete;

  /**
   * @brief 析构函数。
   * @brief Destructor.
   *
   * 默认析构函数确保共享指针的正确清理。
   * Default destructor ensures proper cleanup of the shared pointer.
   */
  ~RWLockData() = default;

  /**
   * @brief 获取共享（读）锁。
   * @brief Acquires a shared (read) lock.
   *
   * 允许多个线程同时读取数据。
   * Allows multiple threads to read the data concurrently.
   */
  void LockRead() { mutex_.lock_shared(); }

  /**
   * @brief 尝试获取共享（读）锁而不阻塞。
   * @brief Attempts to acquire a shared (read) lock without blocking.
   *
   * @return 如果成功获取锁则返回 `true`；否则返回 `false`。
   * @return `true` if the lock was successfully acquired; otherwise, `false`.
   */
  bool TryLockRead() { return mutex_.try_lock_shared(); }

  /**
   * @brief 释放共享（读）锁。
   * @brief Releases a shared (read) lock.
   *
   */
  void UnlockRead() { mutex_.unlock_shared(); }

  /**
   * @brief 获取独占（写）锁。
   * @brief Acquires an exclusive (write) lock.
   *
   * 确保对数据的独占访问，阻塞其他读者和写者。
   * Ensures exclusive access to the data, blocking other readers and writers.
   */
  void LockWrite() { mutex_.lock(); }

  /**
   * @brief 尝试获取独占（写）锁而不阻塞。
   * @brief Attempts to acquire an exclusive (write) lock without blocking.
   *
   * @return 如果成功获取锁则返回 `true`；否则返回 `false`。
   * @return `true` if the lock was successfully acquired; otherwise, `false`.
   */
  bool TryLockWrite() { return mutex_.try_lock(); }

  /**
   * @brief 释放独占（写）锁。
   * @brief Releases an exclusive (write) lock.
   *
   */
  void UnlockWrite() { mutex_.unlock(); }

  /**
   * @brief 将新数据赋值给共享指针。
   * @brief Assigns new data to the shared pointer.
   *
   * 创建一个指向 `data` 副本的新 `std::shared_ptr<T>` 并原子性地赋值。
   * Creates a new `std::shared_ptr<T>` pointing to a copy of `data` and assigns it atomically.
   *
   * @param data 要存储的新数据。
   * @param data The new data to be stored.
   */
  void operator=(const T& data) { data_ptr_ = std::make_shared<T>(data); }

  /**
   * @brief 获取共享指针。
   * @brief Retrieves the shared pointer.
   *
   * @return 指向 `T` 类型对象的共享指针。
   * @return A shared pointer to the object of type `T`.
   */
  std::shared_ptr<T> GetPtr() { return data_ptr_; }

  /**
   * @brief 获取共享指针指向的值。
   * @brief Retrieves the value pointed to by the shared pointer.
   *
   * @return `T` 类型对象的副本。
   * @return A copy of the object of type `T`.
   */
  T GetValue() { return *data_ptr_; }

 private:
  std::shared_ptr<T> data_ptr_; /**< 指向 `T` 类型管理对象的共享指针。<br>Shared pointer to the managed object of type `T` */
  std::shared_mutex mutex_;     /**< 用于管理读写锁的共享互斥锁。<br>Shared mutex for managing read-write locks */
};

}  // namespace openrobot::ocm
