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

SharedMemorySemaphore::SharedMemorySemaphore(const SharedMemorySemaphore&) {
  throw std::logic_error("[SharedMemorySemaphore] Copy constructor is not allowed!");
}

/*!
 * 打开现有的命名信号量，如果信号量不存在，则创建它。
 * @param name 信号量的名字
 * @param value 信号量的初始值
 */
void SharedMemorySemaphore::Init(const std::string& name, unsigned int value) {
  sem_ = sem_open(name.c_str(), O_CREAT, 0644, value);
  if (sem_ == SEM_FAILED) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to initialize shared memory semaphore: " + std::string(strerror(errno)));
  } else {
    name_ = name;
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
    throw std::runtime_error("[SharedMemorySemaphore] Failed to unlink semaphore: " + std::string(strerror(errno)));
  }
}

int SharedMemorySemaphore::GetValue() const {
  int value;
  if (sem_getvalue(sem_, &value) != 0) {
    throw std::runtime_error("[SharedMemorySemaphore] Failed to get semaphore value: " + std::string(strerror(errno)));
  }
  return value;
}

SharedMemoryData::SharedMemoryData(const std::string& name, size_t size) : sem_(name, 1), data_(nullptr), fd_(0) { Init(name, size); }

SharedMemoryData::SharedMemoryData(const SharedMemoryData&) : sem_("", 1) {
  throw std::logic_error("[SharedMemoryData] Copy constructor is not allowed!");
}

void SharedMemoryData::Init(const std::string& name, size_t size) {
  assert(!data_);  // 确保当前没有分配共享内存
  bool is_create = false;
  name_ = name;
  size_ = size;

  // 尝试打开已有的共享内存对象
  fd_ = shm_open(name.c_str(), O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
  if (fd_ == -1) {
    // 如果共享内存对象不存在，创建它
    if (errno == ENOENT) {
      throw std::runtime_error("[SharedMemoryData] Shared memory \"" + name + "\" not found, attempting to create it.");
    } else {
      throw std::runtime_error("[SharedMemoryData] shm_open failed: " + std::string(strerror(errno)));
    }
  } else {
    struct stat s;
    if (fstat(fd_, &s)) {
      throw std::runtime_error("[SharedMemoryData] fstat failed: " + std::string(strerror(errno)));
    }
    if ((size_t)s.st_size != size_) {
      throw std::runtime_error("[SharedMemoryData] Existing shared memory size mismatch!");
    }
  }

  // 如果共享内存不存在, create it
  if (fd_ == -1 && errno == ENOENT) {
    fd_ = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (fd_ == -1) {
      throw std::runtime_error("[SharedMemoryData] Failed to create shared memory: " + std::string(strerror(errno)));
    }
    // 设置共享内存大小
    if (ftruncate(fd_, size_) != 0) {
      throw std::runtime_error("[SharedMemoryData] ftruncate failed: " + std::string(strerror(errno)));
    }
    is_create = true;
  }

  // 映射共享内存
  void* mem = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (mem == MAP_FAILED) {
    throw std::runtime_error("[SharedMemoryData] mmap failed: " + std::string(strerror(errno)));
  }
  // 清零共享内存
  if (is_create) {
    memset(mem, 0, size_);
  }

  data_ = static_cast<uint8_t*>(mem);
}

/*!
 * 释放与当前打开的共享内存对象相关的内存。
 * 该对象可以通过 Init 打开。
 * 调用此方法后，任何进程都不能再使用此共享对象。
 */
void SharedMemoryData::CloseExisting() {
  sem_.Destroy();
  assert(data_);
  // 首先，解除映射
  if (munmap(static_cast<void*>(data_), size_) != 0) {
    throw std::runtime_error("[SharedMemoryData::CloseExisting] munmap failed: " + std::string(strerror(errno)));
  }

  data_ = nullptr;

  if (shm_unlink(name_.c_str()) != 0) {
    throw std::runtime_error("[SharedMemoryData::CloseExisting] shm_unlink failed: " + std::string(strerror(errno)));
  }

  // 关闭文件描述符
  if (close(fd_) != 0) {
    throw std::runtime_error("[SharedMemoryData::CloseExisting] close failed: " + std::string(strerror(errno)));
  }

  fd_ = 0;
}

/*!
 * 关闭当前打开的共享内存对象的视图。
 * 该对象可以通过 Init 打开。
 * 调用此方法后，此进程将无法再使用此共享对象，但其他进程仍然可以。
 */
void SharedMemoryData::Detach() {
  assert(data_);
  // 首先，解除映射
  if (munmap(static_cast<void*>(data_), size_) != 0) {
    throw std::runtime_error("[SharedMemoryData::Detach] munmap failed: " + std::string(strerror(errno)));
  }

  data_ = nullptr;

  // 关闭文件描述符
  if (close(fd_) != 0) {
    throw std::runtime_error("[SharedMemoryData::Detach] close failed: " + std::string(strerror(errno)));
  }

  fd_ = 0;
}

/*!
 * 获取共享内存对象。
 */
uint8_t* SharedMemoryData::Get() {
  assert(data_);
  return data_;
}

void SharedMemoryData::Lock() { sem_.Decrement(); }

void SharedMemoryData::UnLock() { sem_.Increment(); }

}  // namespace openrobot::ocm
