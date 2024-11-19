#include <format>
#include "ocm/ocm.hpp"
#include "task/task.hpp"

class Task : public openrobot::ocm::TaskBase {
 public:
  Task() : openrobot::ocm::TaskBase("openrobot_task", openrobot::ocm::TaskType::TRIGGER, "openrobot_task") {}
  void Run() override { std::cout << std::format("[openrobot_task]{}", this->GetLoopDuration()) << std::endl; }
};

int main() {
  Task timer_task;
  openrobot::ocm::SharedMemorySemaphore sem("openrobot_task", 0);
  timer_task.TaskStart();
  for (int i = 0; i < 10; ++i) {
    sem.Increment();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  timer_task.TaskDestroy();
  return 0;
}
