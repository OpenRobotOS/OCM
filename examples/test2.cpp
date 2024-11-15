#include <fcntl.h>      // For O_* constants
#include <semaphore.h>  // For POSIX named semaphores
#include <sys/stat.h>   // For mode constants
#include <chrono>
#include <cstring>  // For strerror()
#include <iostream>
#include <thread>
#include <unordered_map>

int main() {
  // 创建一个 unordered_map，存储 20 个命名信号量的名称（通过 sem_open 创建）
  std::unordered_map<int, sem_t*> semaphore_map;

  // 创建命名信号量，所有信号量的初始值为 0
  for (int i = 0; i < 200; ++i) {
    std::string semaphore_name = "/semaphore_" + std::to_string(i);         // 创建唯一的命名信号量名
    semaphore_map[i] = sem_open(semaphore_name.c_str(), O_CREAT, 0644, 0);  // 创建信号量，初始值为 0

    // 检查 sem_open 是否成功
    if (semaphore_map[i] == SEM_FAILED) {
      std::cerr << "Error opening semaphore " << semaphore_name << ": " << strerror(errno) << std::endl;
      return 1;
    }
  }

  // 记录开始时间
  auto start_time = std::chrono::high_resolution_clock::now();

  // 执行 20 次 release()
  for (auto& pair : semaphore_map) {
    sem_post(pair.second);  // 释放信号量
  }

  // 记录结束时间
  auto end_time = std::chrono::high_resolution_clock::now();

  // 计算耗时
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  // 输出耗时
  std::cout << "Time to release 20 named POSIX semaphores: " << duration.count() << " microseconds" << std::endl;

  // 关闭和删除信号量
  for (auto& pair : semaphore_map) {
    sem_close(pair.second);  // 关闭信号量
  }

  // 删除命名信号量
  for (int i = 0; i < 20; ++i) {
    std::string semaphore_name = "/semaphore_" + std::to_string(i);
    sem_unlink(semaphore_name.c_str());  // 删除信号量
  }

  return 0;
}
