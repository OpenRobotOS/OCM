#include "MyData.hpp"
#include "ocm/ocm.hpp"

using namespace openrobot::ocm;

int main() {
  SharedMemoryTopic topic;
  MyData data;
  int i;
  while (true) {
    topic.Subscribe<MyData>("topic1", "shm1", [](const MyData& msg) { std::cout << "Received data: " << msg.count << std::endl; });
  }
  return 0;
}
