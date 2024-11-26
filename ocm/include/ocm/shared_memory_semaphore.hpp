// Start of Selection
#pragma once

#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <lcm/lcm-cpp.hpp>
#include <string>

namespace openrobot::ocm {
/**
 * @brief 信号量包装器，用于管理进程间同步。
 * @brief Semaphore wrapper for managing inter-process synchronization.
 *
 * `SharedMemorySemaphore` 类提供了对命名 POSIX 信号量的接口，
 * 允许多个进程同步访问共享资源。
 *
 * The `SharedMemorySemaphore` class provides an interface to named POSIX semaphores,
 * allowing multiple processes to synchronize access to shared resources.
 */
class SharedMemorySemaphore {
 public:
  /**
   * @brief 使用给定的名称和初始值构造 SharedMemorySemaphore。
   * @brief Constructs a SharedMemorySemaphore with the given name and initial value.
   *
   * 通过调用 `Init` 方法初始化信号量。
   *
   * Initializes the semaphore by calling the `Init` method.
   *
   * @param name 信号量的名称标识符。
   * @param name The name identifier for the semaphore.
   * @param value 信号量的初始值。
   * @param value The initial value of the semaphore.
   */
  SharedMemorySemaphore(const std::string& name, unsigned int value);

  /**
   * @brief 析构函数。
   * @brief Destructor.
   *
   * 默认析构函数确保信号量的正确清理。
   *
   * The default destructor ensures proper cleanup of the semaphore.
   */
  ~SharedMemorySemaphore();

  /**
   * @brief 初始化信号量。
   * @brief Initializes the semaphore.
   *
   * 打开一个已存在的命名信号量，如果不存在则创建一个新的信号量。
   *
   * Opens an existing named semaphore or creates a new one if it does not exist.
   *
   * @param sem_name 信号量的名称标识符。
   * @param sem_name The name identifier for the semaphore.
   * @param value 信号量的初始值。
   * @param value The initial value of the semaphore.
   *
   * @throws std::runtime_error 如果信号量初始化失败。
   * @throws std::runtime_error If semaphore initialization fails.
   */
  void Init(const std::string& sem_name, unsigned int value);

  /**
   * @brief 增加信号量的值。
   * @brief Increments the semaphore's value.
   *
   * 等同于 "post" 操作，将信号量的计数增加一。
   *
   * Equivalent to a "post" operation, increasing the semaphore's count by one.
   *
   * @throws std::runtime_error 如果增加操作失败。
   * @throws std::runtime_error If the increment operation fails.
   */
  void Increment();

  /**
   * @brief 如果信号量当前值为零，则增加信号量的值。
   * @brief Increments the semaphore's value if it is currently zero.
   *
   * 检查信号量的当前值，只有在其为零时才增加。
   *
   * Checks the semaphore's current value and only increments it if it is zero.
   *
   * @throws std::runtime_error 如果增加操作失败。
   * @throws std::runtime_error If the increment operation fails.
   */
  void IncrementWhenZero();

  /**
   * @brief 按指定数量增加信号量的值。
   * @brief Increments the semaphore's value by a specified amount.
   *
   * 执行多个 "post" 操作以增加信号量的计数。
   *
   * Performs multiple "post" operations to increase the semaphore's count.
   *
   * @param value 要增加的信号量数量。
   * @param value The amount by which to increment the semaphore.
   *
   * @throws std::runtime_error 如果任何增加操作失败。
   * @throws std::runtime_error If any increment operation fails.
   */
  void Increment(unsigned int value);

  /**
   * @brief 减少信号量的值。
   * @brief Decrements the semaphore's value.
   *
   * 等同于 "wait" 操作，将信号量的计数减少一。
   *
   * Equivalent to a "wait" operation, decreasing the semaphore's count by one.
   *
   * @throws std::runtime_error 如果减少操作失败。
   * @throws std::runtime_error If the decrement operation fails.
   */
  void Decrement();

  /**
   * @brief 尝试在不阻塞的情况下减少信号量的值。
   * @brief Attempts to decrement the semaphore's value without blocking.
   *
   * 尝试执行 "wait" 操作。如果信号量的值大于零，
   * 则减小其值并返回 `true`。否则，返回 `false`。
   *
   * Tries to perform a "wait" operation. If the semaphore's value is greater than zero,
   * it decrements the value and returns `true`. Otherwise, it returns `false`.
   *
   * @return 如果信号量成功减小，则返回 `true`；否则，返回 `false`。
   * @return `true` if the semaphore was successfully decremented; otherwise, `false`.
   */
  bool TryDecrement();

  /**
   * @brief 尝试在超时的情况下减少信号量的值。
   * @brief Attempts to decrement the semaphore's value with a timeout.
   *
   * 等待信号量可用，直到指定的超时时间。
   *
   * Waits for the semaphore to become available until the specified timeout.
   *
   * @param seconds 等待的秒数。
   * @param seconds The number of seconds to wait.
   * @param nanoseconds 等待的纳秒数。
   * @param nanoseconds The number of nanoseconds to wait.
   * @return 如果在超时内成功减小信号量，则返回 `true`；否则，返回 `false`。
   * @return `true` if the semaphore was successfully decremented within the timeout; otherwise, `false`.
   *
   * @throws std::runtime_error 如果获取当前时间失败。
   * @throws std::runtime_error If retrieving the current time fails.
   */
  bool DecrementTimeout(uint64_t seconds, uint64_t nanoseconds);

  /**
   * @brief 获取信号量的当前值。
   * @brief Retrieves the current value of the semaphore.
   *
   * @return 信号量的当前计数。
   * @return The current count of the semaphore.
   *
   * @throws std::runtime_error 如果获取信号量的值失败。
   * @throws std::runtime_error If retrieving the semaphore's value fails.
   */
  int GetValue() const;

  /**
   * @brief 销毁信号量。
   * @brief Destroys the semaphore.
   *
   * 关闭并取消链接信号量，将其从系统中移除。
   *
   * Closes and unlinks the semaphore, removing it from the system.
   *
   * @throws std::runtime_error 如果关闭或取消链接信号量失败。
   * @throws std::runtime_error If closing or unlinking the semaphore fails.
   */
  void Destroy();

 private:
  sem_t* sem_ = nullptr; /**< 指向 POSIX 信号量的指针。Pointer to the POSIX semaphore */
  std::string name_;     /**< 信号量的名称标识符。Name identifier for the semaphore */
};

}  // namespace openrobot::ocm
// End of Selection
