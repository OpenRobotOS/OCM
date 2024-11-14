#include "ocm/ocm.hpp"
#include <iostream>

namespace openrobot::ocm {
SharedMemorySemaphore::SharedMemorySemaphore(const std::string& name, unsigned int value) { Init(name, value); }
SharedMemorySemaphore::SharedMemorySemaphore(const SharedMemorySemaphore&) { throw std::logic_error("[SharedMemorySemaphore]copy construct!"); }
/*!
 * 打开现有的命名信号量，如果信号量不存在，则创建它。
 * @param name 信号量的名字
 * @param value 信号量的初始值
 */
void SharedMemorySemaphore::Init(const std::string& name, unsigned int value) {
  sem_ = sem_open(name.c_str(), O_CREAT, 0644, value);
  if (sem_ == SEM_FAILED) {
    std::cerr << "[ERROR] Failed to initialize shared memory semaphore: " << strerror(errno) << "\n";
  } else {
    name_ = name;
  }
}

/*!
 * 增加信号量的值。
 */
void SharedMemorySemaphore::Increment() {
  if (sem_post(sem_) != 0) {
    std::cerr << "[ERROR] Failed to increment semaphore: " << strerror(errno) << "\n";
  }
}

/*!
 * 等待信号量的值大于0，如果信号量为0，则阻塞等待。
 */
void SharedMemorySemaphore::Decrement() {
  if (sem_wait(sem_) != 0) {
    std::cerr << "[ERROR] Failed to decrement semaphore: " << strerror(errno) << "\n";
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
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_nsec += nanoseconds;
  ts.tv_sec += seconds;
  ts.tv_sec += ts.tv_nsec / 1000000000;
  ts.tv_nsec %= 1000000000;

  return (sem_timedwait(sem_, &ts) == 0);
}

/*!
 * 删除命名信号量，确保其他进程不再使用它。
 */
void SharedMemorySemaphore::Destroy() {
  if (sem_close(sem_) != 0) {
    std::cerr << "[ERROR] Failed to close semaphore: " << strerror(errno) << "\n";
  }
  if (sem_unlink(name_.c_str()) != 0) {
    std::cerr << "[ERROR] Failed to unlink semaphore: " << strerror(errno) << "\n";
  }
}

SharedMemoryData::SharedMemoryData(const std::string& name, size_t size) : sem_(name, 1), data_(nullptr), fd_(0) { Init(name, size); }
SharedMemoryData::SharedMemoryData(const SharedMemoryData&) : sem_("", 1) { throw std::logic_error("[SharedMemoryData]copy construct!"); }

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
      printf("[Shared Memory] %s not found, creating new shared memory, size %ld bytes\n", name.c_str(), size_);
      fd_ = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
      if (fd_ == -1) {
        printf("[ERROR] SharedMemoryManager shm_open failed: %s\n", strerror(errno));
        throw std::runtime_error("Failed to create or attach shared memory!");
      }
      // 设置共享内存大小
      if (ftruncate(fd_, size_)) {
        printf("[ERROR] SharedMemoryManager::Init(%s) ftruncate(%ld): %s\n", name.c_str(), size_, strerror(errno));
        throw std::runtime_error("Failed to create or attach shared memory!");
      }
      is_create = true;
    } else {
      printf("[ERROR] SharedMemoryManager shm_open failed: %s\n", strerror(errno));
      throw std::runtime_error("Failed to create or attach shared memory!");
    }
  } else {
    struct stat s;
    if (fstat(fd_, &s)) {
      printf("[ERROR] SharedMemoryManager::Init(%s) stat: %s\n", name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create or attach shared memory!");
    }
    if ((size_t)s.st_size != size_) throw std::runtime_error("[Shared Memory] Existing shared memory size mismatch!\n");
  }

  // 映射共享内存
  void* mem = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if (mem == MAP_FAILED) {
    printf("[ERROR] SharedMemoryManager::Init(%s) mmap fail: %s\n", name.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
  }
  // 清零共享内存
  if (is_create) memset(mem, 0, size_);

  data_ = (uint8_t*)mem;
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
  if (munmap((void*)data_, size_)) {
    printf("[ERROR] SharedMemoryManager::closeExisting (%s) munmap %s\n", name_.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
    return;
  }

  data_ = nullptr;

  if (shm_unlink(name_.c_str())) {
    printf("[ERROR] SharedMemoryManager::closeExisting (%s) shm_unlink %s\n", name_.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
    return;
  }

  // 关闭文件描述符
  if (close(fd_)) {
    printf("[ERROR] SharedMemoryManager::closeExisting (%s) close %s\n", name_.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
    return;
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
  if (munmap((void*)data_, size_)) {
    printf("[ERROR] SharedMemoryManager::detach (%s) munmap %s\n", name_.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
    return;
  }

  data_ = nullptr;

  // 关闭文件描述符
  if (close(fd_)) {
    printf("[ERROR] SharedMemoryManager::detach (%s) close %s\n", name_.c_str(), strerror(errno));
    throw std::runtime_error("Failed to create or attach shared memory!");
    return;
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
