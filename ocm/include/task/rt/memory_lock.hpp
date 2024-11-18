#pragma once

#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <limits.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <vector>

namespace openrobot::ocm {

inline int lock_memory() {
  if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
    perror("mlockall failed");
    return -1;
  }

  // Turn off malloc trimming.
  if (mallopt(M_TRIM_THRESHOLD, -1) == 0) {
    perror("mallopt for trim threshold failed");
    munlockall();
    return -1;
  }

  // Turn off mmap usage.
  if (mallopt(M_MMAP_MAX, 0) == 0) {
    perror("mallopt for mmap failed");
    mallopt(M_TRIM_THRESHOLD, 128 * 1024);
    munlockall();
    return -1;
  }
  return 0;
}

inline int lock_and_prefault_dynamic() {
  if (lock_memory() != 0) {
    return -1;
  }

  struct rusage usage;
  size_t page_size = sysconf(_SC_PAGESIZE);
  getrusage(RUSAGE_SELF, &usage);
  std::vector<char*> prefaulter;
  size_t prev_minflts = usage.ru_minflt;
  size_t prev_majflts = usage.ru_majflt;
  size_t encountered_minflts = 1;
  size_t encountered_majflts = 1;
  // prefault until you see no more pagefaults
  while (encountered_minflts > 0 || encountered_majflts > 0) {
    char* ptr;
    try {
      ptr = new char[64 * page_size];
      memset(ptr, 0, 64 * page_size);
    } catch (std::bad_alloc& e) {
      fprintf(stderr, "Caught exception: %s\n", e.what());
      fprintf(stderr, "Unlocking memory and continuing.\n");
      for (auto& ptr : prefaulter) {
        delete[] ptr;
      }

      mallopt(M_TRIM_THRESHOLD, 128 * 1024);
      mallopt(M_MMAP_MAX, 65536);
      munlockall();
      return -1;
    }
    prefaulter.push_back(ptr);
    getrusage(RUSAGE_SELF, &usage);
    size_t current_minflt = usage.ru_minflt;
    size_t current_majflt = usage.ru_majflt;
    encountered_minflts = current_minflt - prev_minflts;
    encountered_majflts = current_majflt - prev_majflts;
    prev_minflts = current_minflt;
    prev_majflts = current_majflt;
  }

  for (auto& ptr : prefaulter) {
    delete[] ptr;
  }
  return 0;
}

inline int lock_and_prefault_dynamic(size_t process_max_dynamic_memory) {
  if (lock_memory() != 0) {
    return -1;
  }

  void* buf = nullptr;
  const size_t pg_sz = sysconf(_SC_PAGESIZE);
  int res;
  res = posix_memalign(&buf, static_cast<size_t>(pg_sz), process_max_dynamic_memory);
  if (res != 0) {
    std::cerr << "proc rt init mem aligning failed" << strerror(errno) << std::endl;
    throw std::runtime_error("proc rt init mem aligning failed");
  }
  memset(buf, 0, process_max_dynamic_memory);
  free(buf);

  return 0;
}

}  // namespace openrobot::ocm