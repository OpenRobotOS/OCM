#include <iostream>
#include <mutex>

// 单例类
class Singleton {
 public:
  // 删除拷贝构造函数和赋值运算符，防止复制
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

  // 提供一个静态方法来获取单例实例
  static Singleton& getInstance() {
    // C++11及以上保证静态局部变量的线程安全初始化
    static Singleton instance;
    return instance;
  }

  // 示例成员函数
  void doSomething() { std::cout << "Singleton instance address: " << this << std::endl; }

 private:
  // 私有构造函数，防止外部实例化
  Singleton() { std::cout << "Singleton constructor called." << std::endl; }

  // 可选：私有析构函数
  ~Singleton() { std::cout << "Singleton destructor called." << std::endl; }
};

// 示例多线程环境
#include <thread>

void threadFunc(int id) {
  Singleton& singleton = Singleton::getInstance();
  std::cout << "Thread " << id << " accessing singleton at " << &singleton << std::endl;
  singleton.doSomething();
}

int main() {
  const int numThreads = 5;
  std::thread threads[numThreads];

  // 创建多个线程同时访问单例
  for (int i = 0; i < numThreads; ++i) {
    threads[i] = std::thread(threadFunc, i);
  }

  // 等待所有线程完成
  for (int i = 0; i < numThreads; ++i) {
    threads[i].join();
  }

  return 0;
}
