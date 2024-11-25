#pragma once

#include <linux/types.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <set>
#include <string>
#include <vector>

namespace openrobot::ocm::rt {

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_ISO 4
#define SCHED_IDLE 5
#define SCHED_DEADLINE 6

// Define syscall numbers if not already defined
#ifndef SYS_sched_setattr
  #ifdef __x86_64__
    #define SYS_sched_setattr 314
    #define SYS_sched_getattr 315
  #elif defined(__i386__)
    #define SYS_sched_setattr 351
    #define SYS_sched_getattr 352
  #elif defined(__arm__)
    #define SYS_sched_setattr 380
    #define SYS_sched_getattr 381
  #elif defined(__aarch64__)
    #define SYS_sched_setattr 274
    #define SYS_sched_getattr 275
  #else
    #error "Unsupported architecture"
  #endif
#endif

#define SF_SIG_RORUN 2
#define SF_SIG_DMISS 4
#define SF_BWRECL_DL 8
#define SF_BWRECL_RT 16
#define SF_BWRECL_OTH 32

#define RLIMIT_DLDLINE 16
#define RLIMIT_DLRTIME 17
struct sched_attr_t {
  __u32 size;

  __u32 sched_policy;
  __u64 sched_flags;

  /* SCHED_NORMAL, SCHED_BATCH */
  __s32 sched_nice;

  /* SCHED_FIFO, SCHED_RR */
  __u32 sched_priority;

  /* SCHED_DEADLINE */
  __u64 sched_runtime;
  __u64 sched_deadline;
  __u64 sched_period;
};

inline int sched_setattr(const pid_t pid, const sched_attr_t* attr, const unsigned int flags) { return syscall(SYS_sched_setattr, pid, attr, flags); }

inline int sched_getattr(const pid_t pid, sched_attr_t* attr, unsigned int size, const unsigned int flags) {
  return syscall(SYS_sched_getattr, pid, attr, size, flags);
}

inline int set_thread_priority(const pid_t pid, size_t const sched_priority, const int policy) {
  struct sched_param param;
  memset(&param, 0, sizeof(param));
  param.sched_priority = sched_priority;
  return sched_setscheduler(pid, policy, &param);
}

// 设置线程 CPU 亲和性
inline int set_thread_cpu_affinity(const pid_t pid, const std::vector<int>& cpu_list) {
  cpu_set_t set;
  CPU_ZERO(&set);

  // 获取系统可用的 CPU 数量
  long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
  if (num_cpus == -1) {
    perror("sysconf");
    return -1;
  }

  // 使用 std::set 来自动处理重复值
  std::set<int> unique_cpus(cpu_list.begin(), cpu_list.end());

  for (const int cpu : unique_cpus) {
    // 检查 CPU 编号是否在有效范围内
    if (cpu < 0 || cpu >= num_cpus) {
      fprintf(stderr, "无效的 CPU 编号: %d\n", cpu);
      return -1;
    }
    CPU_SET(cpu, &set);
  }

  // 设置 CPU 亲和性
  if (sched_setaffinity(pid, sizeof(set), &set) == -1) {
    perror("sched_setaffinity");
    return -1;
  }

  return 0;
}
inline void set_thread_name(const std::string& name) {
  const auto iErr = prctl(PR_SET_NAME, name.c_str());
  (void)iErr;
}

}  // namespace openrobot::ocm::rt
