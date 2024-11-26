
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <ctime>
#include <stdexcept>
#include "common/prefix_string.hpp"
#include "ocm/shared_memory_semaphore.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a SharedMemorySemaphore with the given name and initial value.
 * @brief 使用给定的名称和初始值构造 SharedMemorySemaphore。
 *
 * Initializes the semaphore by calling the `Init` method.
 * 通过调用 `Init` 方法初始化信号量。
 *
 * @param name The name identifier for the semaphore.
 * @param name 信号量的名称标识符。
 * @param value The initial value of the semaphore.
 * @param value 信号量的初始值。
 */
SharedMemorySemaphore::SharedMemorySemaphore(const std::string& name, unsigned int value) { Init(name, value); }

/**
 * @brief Opens an existing named semaphore or creates it if it does not exist.
 * @brief 打开一个已存在的命名信号量，或在不存在时创建它。
 *
 * This method attempts to open a named semaphore. If the semaphore does not exist,
 * it creates a new one with the specified initial value.
 * 该方法尝试打开一个命名信号量。如果信号量不存在，
 * 则使用指定的初始值创建一个新的信号量。
 *
 * @param name The name identifier for the semaphore.
 * @param name 信号量的名称标识符。
 * @param value The initial value of the semaphore.
 * @param value 信号量的初始值。
 *
 * @throws std::runtime_error If semaphore initialization fails.
 * @throws std::runtime_error 如果信号量初始化失败。
 */
void SharedMemorySemaphore::Init(const std::string& name, unsigned int value) {
  const auto sem_name = GetNamePrefix(name);
  sem_ = sem_open(sem_name.c_str(), O_CREAT, 0644, value);
  if (sem_ == SEM_FAILED) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to initialize shared memory semaphore: " + std::string(strerror(errno)));
  } else {
    name_ = sem_name;
  }
}

/**
 * @brief Increments the semaphore's value.
 * @brief 增加信号量的值。
 *
 * Equivalent to a "post" operation, increasing the semaphore's count by one.
 * 等同于 "post" 操作，使信号量的计数增加一。
 *
 * @throws std::runtime_error If the increment operation fails.
 * @throws std::runtime_error 如果增加操作失败。
 */
void SharedMemorySemaphore::Increment() {
  if (sem_post(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
  }
}

/**
 * @brief Increments the semaphore's value if it is currently zero.
 * @brief 如果信号量当前为零，则增加其值。
 *
 * Checks the semaphore's current value and only increments it if it is zero.
 * 检查信号量的当前值，仅在其为零时增加它。
 *
 * @throws std::runtime_error If the increment operation fails.
 * @throws std::runtime_error 如果增加操作失败。
 */
void SharedMemorySemaphore::IncrementWhenZero() {
  int value;
  if (sem_getvalue(sem_, &value) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));
  }
  if (value == 0) {
    if (sem_post(sem_) != 0) {
      throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
    }
  }
}

/**
 * @brief Increments the semaphore's value by a specified amount.
 * @brief 按指定数量增加信号量的值。
 *
 * Performs multiple "post" operations to increase the semaphore's count.
 * 执行多个 "post" 操作以增加信号量的计数。
 *
 * @param value The amount by which to increment the semaphore.
 * @param value 增加信号量的数量。
 *
 * @throws std::runtime_error If any increment operation fails.
 * @throws std::runtime_error 如果任何增加操作失败。
 */
void SharedMemorySemaphore::Increment(unsigned int value) {
  for (unsigned int i = 0; i < value; ++i) {
    if (sem_post(sem_) != 0) {
      throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
    }
  }
}

/**
 * @brief Decrements the semaphore's value.
 * @brief 减少信号量的值。
 *
 * Equivalent to a "wait" operation, decreasing the semaphore's count by one.
 * 等同于 "wait" 操作，使信号量的计数减少一。
 *
 * @throws std::runtime_error If the decrement operation fails.
 * @throws std::runtime_error 如果减少操作失败。
 */
void SharedMemorySemaphore::Decrement() {
  if (sem_wait(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to decrement semaphore: " + std::string(strerror(errno)));
  }
}

/**
 * @brief Attempts to decrement the semaphore's value without blocking.
 * @brief 尝试在不阻塞的情况下减少信号量的值。
 *
 * Tries to perform a "wait" operation. If the semaphore's value is greater than zero,
 * it decrements the value and returns `true`. Otherwise, it returns `false`.
 * 尝试执行一个 "wait" 操作。如果信号量的值大于零，
 * 则减少该值并返回 `true`。否则，返回 `false`。
 *
 * @return `true` if the semaphore was successfully decremented; otherwise, `false`.
 * @return 如果信号量成功减少，则返回 `true`；否则，返回 `false`。
 */
bool SharedMemorySemaphore::TryDecrement() { return (sem_trywait(sem_) == 0); }

/**
 * @brief Attempts to decrement the semaphore's value with a timeout.
 * @brief 尝试在超时的情况下减少信号量的值。
 *
 * Waits for the semaphore to become available until the specified timeout.
 * 等待信号量可用，直到指定的超时。
 *
 * @param seconds The number of seconds to wait.
 * @param seconds 等待的秒数。
 * @param nanoseconds The number of nanoseconds to wait.
 * @param nanoseconds 等待的纳秒数。
 * @return `true` if the semaphore was successfully decremented within the timeout; otherwise, `false`.
 * @return 如果在超时时间内成功减少信号量，则返回 `true`；否则，返回 `false`。
 *
 * @throws std::runtime_error If retrieving the current time fails.
 * @throws std::runtime_error 如果获取当前时间失败。
 */
bool SharedMemorySemaphore::DecrementTimeout(uint64_t seconds, uint64_t nanoseconds) {
  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get current time: " + std::string(strerror(errno)));
  }

  ts.tv_sec += seconds;
  ts.tv_nsec += nanoseconds;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  return (sem_timedwait(sem_, &ts) == 0);
}

/**
 * @brief Retrieves the current value of the semaphore.
 * @brief 获取信号量的当前值。
 *
 * @return The current count of the semaphore.
 * @return 信号量的当前计数。
 *
 * @throws std::runtime_error If retrieving the semaphore's value fails.
 * @throws std::runtime_error 如果获取信号量的值失败。
 */
int SharedMemorySemaphore::GetValue() const {
  int value;
  if (sem_getvalue(sem_, &value) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));
  }
  return value;
}

/**
 * @brief Destroys the semaphore.
 * @brief 销毁信号量。
 *
 * Closes and unlinks the semaphore, removing it from the system.
 * 关闭并取消链接信号量，将其从系统中移除。
 *
 * @throws std::runtime_error If closing or unlinking the semaphore fails.
 * @throws std::runtime_error 如果关闭或取消链接信号量失败。
 */
void SharedMemorySemaphore::Destroy() {
  if (sem_close(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to close semaphore: " + std::string(strerror(errno)));
  }
  if (sem_unlink(name_.c_str()) != 0) {
    if (errno != ENOENT) {
      throw std::runtime_error("[SharedMemorySemaphore] Failed to unlink semaphore: " + std::string(strerror(errno)));
    }
  }
}

/**
 * @brief Destructor for SharedMemorySemaphore.
 * @brief SharedMemorySemaphore 的析构函数。
 *
 * Ensures that the semaphore is properly destroyed when the object is destructed.
 * 确保在对象被销毁时，信号量被正确销毁。
 */
SharedMemorySemaphore::~SharedMemorySemaphore() = default;

}  // namespace openrobot::ocm
