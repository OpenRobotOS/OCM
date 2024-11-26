#include "ocm/ocm.hpp"
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <stdexcept>

namespace openrobot::ocm {

/**
 * @brief Constructs a SharedMemorySemaphore with the given name and initial value.
 *
 * Initializes the semaphore by calling the `Init` method.
 *
 * @param name The name identifier for the semaphore.
 * @param value The initial value of the semaphore.
 */
SharedMemorySemaphore::SharedMemorySemaphore(const std::string& name, unsigned int value) { Init(name, value); }

/**
 * @brief Opens an existing named semaphore or creates it if it does not exist.
 *
 * This method attempts to open a named semaphore. If the semaphore does not exist,
 * it creates a new one with the specified initial value.
 *
 * @param name The name identifier for the semaphore.
 * @param value The initial value of the semaphore.
 *
 * @throws std::runtime_error If semaphore initialization fails.
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
 *
 * Equivalent to a "post" operation, increasing the semaphore's count by one.
 *
 * @throws std::runtime_error If the increment operation fails.
 */
void SharedMemorySemaphore::Increment() {
  if (sem_post(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
  }
}

/**
 * @brief Increments the semaphore's value if it is currently zero.
 *
 * Checks the semaphore's current value and only increments it if it is zero.
 *
 * @throws std::runtime_error If the increment operation fails.
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
 *
 * Performs multiple "post" operations to increase the semaphore's count.
 *
 * @param value The amount by which to increment the semaphore.
 *
 * @throws std::runtime_error If any increment operation fails.
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
 *
 * Equivalent to a "wait" operation, decreasing the semaphore's count by one.
 *
 * @throws std::runtime_error If the decrement operation fails.
 */
void SharedMemorySemaphore::Decrement() {
  if (sem_wait(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to decrement semaphore: " + std::string(strerror(errno)));
  }
}

/**
 * @brief Attempts to decrement the semaphore's value without blocking.
 *
 * Tries to perform a "wait" operation. If the semaphore's value is greater than zero,
 * it decrements the value and returns `true`. Otherwise, it returns `false`.
 *
 * @return `true` if the semaphore was successfully decremented; otherwise, `false`.
 */
bool SharedMemorySemaphore::TryDecrement() { return (sem_trywait(sem_) == 0); }

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
 *
 * @return The current count of the semaphore.
 *
 * @throws std::runtime_error If retrieving the semaphore's value fails.
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
 *
 * Closes and unlinks the semaphore, removing it from the system.
 *
 * @throws std::runtime_error If closing or unlinking the semaphore fails.
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
 *
 * Ensures that the semaphore is properly destroyed when the object is destructed.
 */
SharedMemorySemaphore::~SharedMemorySemaphore() = default;

}  // namespace openrobot::ocm
