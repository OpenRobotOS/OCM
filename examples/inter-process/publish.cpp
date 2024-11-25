#include <thread>
#include "MyData.hpp"
#include "ocm/ocm.hpp"
using namespace openrobot::ocm;
int main() {
  SharedMemoryTopic topic;
  MyData data;
  data.count = 1;
  for (int i = 0; i < 10; i++) {
    data.values[i] = i;
  }
  topic.Publish("topic1", "shm1", &data);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
