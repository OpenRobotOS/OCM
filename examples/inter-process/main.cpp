#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

const char* SEM_NAME = "/openrobot_ocm_sem";  // 命名信号量的名字

// 订阅者函数
void subscriber(int id, sem_t* sem) {
  std::cout << "订阅者 " << id << " 等待信号...\n";

  // 等待信号量
  sem_wait(sem);

  // 接收到信号后处理任务
  std::cout << "订阅者 " << id << " 收到信号，开始处理任务...\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));  // 模拟处理时间

  std::cout << "订阅者 " << id << " 处理完成。\n";
}

// 发布者函数
void publisher(sem_t* sem) {
  std::cout << "发布者发布信号...\n";

  // 发布信号量，唤醒所有等待的订阅者
  sem_post(sem);
}

int main() {
  // 创建命名信号量，初始值为 0（表示没有信号）
  sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
  if (sem == SEM_FAILED) {
    std::cerr << "sem_open failed: " << strerror(errno) << "\n";
    return -1;
  }

  // 启动多个订阅者线程
  const int num_subscribers = 3;
  std::vector<std::thread> subscribers;
  for (int i = 0; i < num_subscribers; ++i) {
    subscribers.push_back(std::thread(subscriber, i + 1, sem));
  }

  // 启动发布者线程
  std::thread pub_thread(publisher, sem);

  // 等待所有订阅者和发布者线程完成
  pub_thread.join();
  for (auto& t : subscribers) {
    t.join();
  }

  // 关闭并删除命名信号量
  sem_close(sem);
  sem_unlink(SEM_NAME);

  return 0;
}
