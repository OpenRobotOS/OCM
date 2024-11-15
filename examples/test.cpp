#include <chrono>
#include <iostream>
#include <memory>     // For std::shared_ptr
#include <semaphore>  // For std::binary_semaphore
#include <thread>
#include <unordered_map>

int main() {
  // 创建一个 unordered_map，存储 20 个 std::shared_ptr<std::binary_semaphore>（初始值为 0）
  std::unordered_map<int, std::shared_ptr<std::binary_semaphore>> semaphore_map;

  // 初始化信号量，所有信号量的初始值为 0
  for (int i = 0; i < 200; ++i) {
    semaphore_map[i] = std::make_shared<std::binary_semaphore>(0);  // 使用 std::shared_ptr 包装
  }

  // 记录开始时间
  auto start_time = std::chrono::high_resolution_clock::now();

  // 执行 20 次 release()
  for (auto& pair : semaphore_map) {
    pair.second->release();  // 使用指针访问 release()
  }

  // 记录结束时间
  auto end_time = std::chrono::high_resolution_clock::now();

  // 计算耗时
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  // 输出耗时
  std::cout << "Time to release 20 semaphores: " << duration.count() << " microseconds" << std::endl;

  return 0;
}
