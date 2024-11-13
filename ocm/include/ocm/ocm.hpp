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
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>

namespace openrobot::ocm {

/*!
 * shared memory
 */

class SharedMemorySemaphore {
 public:
  /*!
   * 打开现有的命名信号量，如果信号量不存在，则创建它。
   * @param sem_name 信号量的名字
   * @param value 信号量的初始值
   */
  void init(const std::string& sem_name, unsigned int value) {
    if (!_init) {
      _sem = sem_open(sem_name.c_str(), O_CREAT, 0644, value);
      if (_sem == SEM_FAILED) {
        std::cerr << "[ERROR] Failed to initialize shared memory semaphore: " << strerror(errno) << "\n";
      } else {
        _init = true;
        _sem_name = sem_name;
      }
    }
  }

  /*!
   * 增加信号量的值。
   */
  void increment() {
    if (sem_post(_sem) != 0) {
      std::cerr << "[ERROR] Failed to increment semaphore: " << strerror(errno) << "\n";
    }
  }

  /*!
   * 等待信号量的值大于0，如果信号量为0，则阻塞等待。
   */
  void decrement() {
    if (sem_wait(_sem) != 0) {
      std::cerr << "[ERROR] Failed to decrement semaphore: " << strerror(errno) << "\n";
    }
  }

  /*!
   * 尝试减少信号量的值，如果信号量为0，则不会阻塞，直接返回 false。
   * @return 如果成功减少信号量返回 true，否则返回 false。
   */
  bool tryDecrement() { return (sem_trywait(_sem) == 0); }

  /*!
   * 设置超时的等待信号量功能。
   * @param seconds 等待的秒数。
   * @param nanoseconds 等待的纳秒数。
   * @return 如果在超时前成功获得信号量，则返回 true；否则返回 false。
   */
  bool decrementTimeout(uint64_t seconds, uint64_t nanoseconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += nanoseconds;
    ts.tv_sec += seconds;
    ts.tv_sec += ts.tv_nsec / 1000000000;
    ts.tv_nsec %= 1000000000;

    return (sem_timedwait(_sem, &ts) == 0);
  }

  /*!
   * 删除命名信号量，确保其他进程不再使用它。
   */
  void destroy() {
    if (sem_close(_sem) != 0) {
      std::cerr << "[ERROR] Failed to close semaphore: " << strerror(errno) << "\n";
    }
    if (sem_unlink(_sem_name.c_str()) != 0) {
      std::cerr << "[ERROR] Failed to unlink semaphore: " << strerror(errno) << "\n";
    }
  }

 private:
  sem_t* _sem = nullptr;  // 信号量
  std::string _sem_name;  // 信号量的名字
  bool _init = false;     // 是否初始化
};

/*!
 * A container class for an object which is stored in shared memory.  This
 * object can then be viewed in multiple processes or programs.  Note that there
 * is significant overhead when creating a shared memory object, so it is
 * recommended that two programs that communicate should have one single large
 * SharedMemoryObject instead of many small ones.
 *
 * A name string is used to identify shared objects across different programs
 *
 * Before a shared memory object can be used, you must either allocate new
 * memory, or connect it to an existing shared memory object.
 *
 * Creating/deleting the memory can be done with createNew/closeNew.
 * Viewing an existing object allocated with createNew can be done with
 * attach/detach
 *
 * For an example, see test_sharedMemory.cpp
 */
template <typename T>
class SharedMemoryObject {
 public:
  SharedMemoryObject() = default;

  /*!
   * Allocate memory for the shared memory object and attach to it.
   * If allowOverwrite is true, and there's already an object with this name,
   * the old object is overwritten Note that if this happens, the object may be
   * initialized in a very weird state.
   *
   * Otherwise, if an object with the name already exists, throws a
   * std::runtime_error
   */
  bool createNew(const std::string& name, bool allowOverwrite = false) {
    bool hadToDelete = false;
    assert(!_data);
    _name = name;
    _size = sizeof(T);
    printf("[Shared Memory] open new %s, size %ld bytes\n", name.c_str(), _size);

    _fd = shm_open(name.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (_fd == -1) {
      printf("[ERROR] SharedMemoryObject shm_open failed: %s\n", strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return false;
    }

    struct stat s;
    if (fstat(_fd, &s)) {
      printf("[ERROR] SharedMemoryObject::createNew(%s) stat: %s\n", name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return false;
    }

    if (s.st_size) {
      printf(
          "[Shared Memory] SharedMemoryObject::createNew(%s) on something that "
          "wasn't new (size is %ld bytes)\n",
          _name.c_str(), s.st_size);
      hadToDelete = true;
      if (!allowOverwrite) throw std::runtime_error("Failed to create shared memory - it already exists.");

      printf("\tusing existing shared memory!\n");
      // return false;
    }

    if (ftruncate(_fd, _size)) {
      printf("[ERROR] SharedMemoryObject::createNew(%s) ftruncate(%ld): %s\n", name.c_str(), _size, strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return false;
    }

    void* mem = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (mem == MAP_FAILED) {
      printf("[ERROR] SharedMemory::createNew(%s) mmap fail: %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return false;
    }

    // there is a chance that the shared memory is not zeroed if we are reusing
    // old memory. this causes all sorts of weird issues, especially if the
    // layout of the object in memory has changed.
    memset(mem, 0, _size);

    _data = (T*)mem;
    return hadToDelete;
  }

  /*!
   * Attach to an existing shared memory object.
   */
  void attach(const std::string& name) {
    assert(!_data);
    _name = name;
    _size = sizeof(T);
    printf("[Shared Memory] open existing %s size %ld bytes\n", name.c_str(), _size);
    _fd = shm_open(name.c_str(), O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (_fd == -1) {
      printf("[ERROR] SharedMemoryObject::attach shm_open(%s) failed: %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    struct stat s;
    if (fstat(_fd, &s)) {
      printf("[ERROR] SharedMemoryObject::attach(%s) stat: %s\n", name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    if ((size_t)s.st_size != _size) {
      printf(
          "[ERROR] SharedMemoryObject::attach(%s) on something that was "
          "incorrectly "
          "sized (size is %ld bytes, should be %ld)\n",
          _name.c_str(), s.st_size, _size);
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    void* mem = mmap(nullptr, _size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (mem == MAP_FAILED) {
      printf("[ERROR] SharedMemory::attach(%s) mmap fail: %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    _data = (T*)mem;
  }

  /*!
   * Free memory associated with the current open shared memory object.  The
   * object could have been opened with either attach or createNew.  After
   * calling this, no process can use this shared object
   */
  void closeNew() {
    assert(_data);
    // first, unmap
    if (munmap((void*)_data, _size)) {
      printf("[ERROR] SharedMemoryObject::closeNew (%s) munmap %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    _data = nullptr;

    if (shm_unlink(_name.c_str())) {
      printf("[ERROR] SharedMemoryObject::closeNew (%s) shm_unlink %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    // close fd
    if (close(_fd)) {
      printf("[ERROR] SharedMemoryObject::closeNew (%s) close %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    _fd = 0;
  }

  /*!
   * Close this view of the currently opened shared memory object. The object
   * can be opened with either attach or createNew.  After calling this, this
   * process can no longer use this shared object, but other processes still
   * can.
   */
  void detach() {
    assert(_data);
    // first, unmap
    if (munmap((void*)_data, _size)) {
      printf("[ERROR] SharedMemoryObject::detach (%s) munmap %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    _data = nullptr;

    // close fd
    if (close(_fd)) {
      printf("[ERROR] SharedMemoryObject::detach (%s) close %s\n", _name.c_str(), strerror(errno));
      throw std::runtime_error("Failed to create shared memory!");
      return;
    }

    _fd = 0;
  }

  /*!
   * Get the shared memory object.
   */
  T* get() {
    assert(_data);
    return _data;
  }

  /*!
   * Get the shared memory object.
   */
  T& operator()() {
    assert(_data);
    return *_data;
  }

 private:
  T* _data = nullptr;
  std::string _name;
  size_t _size;
  int _fd;
};

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
  RWLockData() = default;
  explicit RWLockData(const T& data) : data_(data) {}
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
  T data_;

 private:
  std::shared_mutex mutex_;
};

/*!
 * inter-process
 */

}  // namespace openrobot::ocm