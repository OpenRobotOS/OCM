#include <sys/epoll.h>  // For epoll
#include <unistd.h>     // For pipe(), STDIN_FILENO
#include <cstring>      // For strerror()
#include <iostream>
#include <thread>

void busy_waiting_function(int pipe_fd) {
  std::cout << "函数开始阻塞，等待信号或I/O事件...\n";

  // 创建 epoll 实例
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    std::cerr << "epoll_create1 failed: " << strerror(errno) << "\n";
    return;
  }

  // 设置监听的标准输入文件描述符
  struct epoll_event ev;
  ev.events = EPOLLIN;  // 监听可读事件
  ev.data.fd = STDIN_FILENO;

  // 将标准输入文件描述符添加到 epoll 实例中
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
    std::cerr << "epoll_ctl failed: " << strerror(errno) << "\n";
    return;
  }

  // 设置监听管道的文件描述符
  ev.data.fd = pipe_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd, &ev) == -1) {
    std::cerr << "epoll_ctl failed: " << strerror(errno) << "\n";
    return;
  }

  struct epoll_event events[2];  // We are monitoring 2 file descriptors (stdin and pipe)
  while (true) {
    // 使用 epoll 等待事件发生
    int ret = epoll_wait(epoll_fd, events, 2, -1);  // 阻塞，直到有事件发生

    if (ret > 0) {
      for (int i = 0; i < ret; ++i) {
        if (events[i].data.fd == STDIN_FILENO) {
          // 检测到标准输入事件
          std::cout << "检测到标准输入事件！\n";
          char buf[256];
          if (fgets(buf, sizeof(buf), stdin)) {
            std::cout << "收到输入：" << buf;
          }
        } else if (events[i].data.fd == pipe_fd) {
          // 检测到管道事件（信号）
          std::cout << "检测到信号，继续执行。\n";
          return;  // 收到信号后退出循环
        }
      }
    } else if (ret == 0) {
      // 超时，没有事件发生
      std::cout << "超时，未检测到事件。\n";
    } else {
      std::cerr << "epoll_wait failed: " << strerror(errno) << "\n";
    }
  }

  // 关闭 epoll 实例
  close(epoll_fd);
}

void send_signal_after_delay(int pipe_fd, int delay_seconds) {
  std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
  const char* signal = "signal\n";
  write(pipe_fd, signal, strlen(signal));  // 向管道写入信号
  std::cout << "信号已发送。\n";
}

int main() {
  // 创建一个管道
  int pipe_fd[2];
  if (pipe(pipe_fd) == -1) {
    std::cerr << "pipe failed: " << strerror(errno) << "\n";
    return -1;
  }

  // 启动等待线程
  std::thread waiter(busy_waiting_function, pipe_fd[0]);

  // 启动发送信号线程
  std::thread signaler(send_signal_after_delay, pipe_fd[1], 50);  // 5秒后发送信号

  // 等待线程完成
  waiter.join();
  signaler.join();

  // 关闭管道
  close(pipe_fd[0]);
  close(pipe_fd[1]);

  return 0;
}
