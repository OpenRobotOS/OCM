#include <cstring>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <memory>
#include <stdexcept>
#include "./MyData.hpp"  // 你的 LCM 消息头文件
#include "ocm/ocm.hpp"

int main() {
  // 创建 LCM 对象

  // 创建 LCM 消息并序列化
  MyData msg;
  msg.count = 42;
  for (int i = 0; i < 10; i++) {
    msg.values[i] = i * 2;
  }

  // 获取消息的序列化大小
  int msg_size = msg.getEncodedSize();

  char* buffer = new char[msg_size];

  // 序列化消息
  msg.encode(buffer, 0, msg_size);

  // 创建共享内存对象并指定大小
  openrobot::ocm::SharedMemoryObject<char> shm(msg_size);  // 使用序列化后的数据大小来创建共享内存对象

  // 分配共享内存
  shm.createNew("SharedMemory");

  // 将序列化的数据复制到共享内存中
  memcpy(shm.get(), buffer, msg_size);

  std::cout << "Data written to shared memory." << std::endl;

  // 模拟另一个进程读取共享内存并反序列化数据
  MyData new_msg;
  new_msg.decode(shm.get(), 0, msg_size);  // 解码共享内存中的数据

  // 输出反序列化后的数据
  std::cout << "Count: " << new_msg.count << std::endl;
  std::cout << "Values: ";
  for (int i = 0; i < 10; i++) {
    std::cout << new_msg.values[i] << " ";
  }
  std::cout << std::endl;

  // 关闭共享内存
  shm.closeNew();

  return 0;
}
