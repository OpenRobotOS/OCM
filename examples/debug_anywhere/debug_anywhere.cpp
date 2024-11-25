#include "debug_anywhere/debug_anywhere.hpp"
#include <thread>
using namespace openrobot::ocm;
int main() {
  auto& debug_anywhere = DebugAnywhere::getInstance();
  debug_anywhere.initialize(DebugAnywhereConfig{"192.168.1.100", "10000", "1"});

  for (int i = 0; i < 10; i++) {
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
    debug_anywhere.Publish("topic1", data);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}
