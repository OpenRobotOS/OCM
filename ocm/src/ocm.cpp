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

SharedMemorySemaphore::SharedMemorySemaphore(const std::string& name, unsigned int value) { Init(name, value); }
/*!
 * 打开现有的命名信号量，如果信号量不存在，则创建它。
 * @param name 信号量的名字
 * @param value 信号量的初始值
 */
void SharedMemorySemaphore::Init(const std::string& name, unsigned int value) {
  const auto sem_name = "openrobot_ocm_" + name;
  sem_ = sem_open(sem_name.c_str(), O_CREAT, 0644, value);
  if (sem_ == SEM_FAILED) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to initialize shared memory semaphore: " + std::string(strerror(errno)));
  } else {
    name_ = sem_name;
  }
}

/*!
 * 增加信号量的值。
 */
void SharedMemorySemaphore::Increment() {
  if (sem_post(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
  }
}

/*!
 * 增加信号量的值，当信号量为零时。
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

/*!
 * 增加信号量的值。
 */
void SharedMemorySemaphore::Increment(unsigned int value) {
  for (unsigned int i = 0; i < value; ++i) {
    if (sem_post(sem_) != 0) {
      throw std::runtime_error("[SharedMemorySemaphore] Failed to increment semaphore: " + std::string(strerror(errno)));
    }
  }
}

/*!
 * 等待信号量的值大于0，如果信号量为0，则阻塞等待。
 */
void SharedMemorySemaphore::Decrement() {
  if (sem_wait(sem_) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to decrement semaphore: " + std::string(strerror(errno)));
  }
}

/*!
 * 尝试减少信号量的值，如果信号量为0，则不会阻塞，直接返回 false。
 * @return 如果成功减少信号量返回 true，否则返回 false。
 */
bool SharedMemorySemaphore::TryDecrement() { return (sem_trywait(sem_) == 0); }

/*!
 * 设置超时的等待信号量功能。
 * @param seconds 等待的秒数。
 * @param nanoseconds 等待的纳秒数。
 * @return 如果在超时前成功获得信号量，则返回 true；否则返回 false。
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

/*!
 * 删除命名信号量，确保其他进程不再使用它。
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

int SharedMemorySemaphore::GetValue() const {
  int value;
  if (sem_getvalue(sem_, &value) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));
  }
  return value;
}

}  // namespace openrobot::ocm
