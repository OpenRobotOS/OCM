#include <format>
#include "task/task.hpp"

class Task : public openrobot::ocm::TaskBase {
 public:
  Task() : openrobot::ocm::TaskBase("openrobot_task", openrobot::ocm::TaskType::INTERNAL_TIMER, 0.0) {}
  void Run() override { std::cout << std::format("[openrobot_task]{}", this->GetLoopDuration()) << std::endl; }
};

int main() {
  Task timer_task;
  timer_task.TaskStart();
  timer_task.SetPeriod(0.001);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  timer_task.TaskDestroy();

  return 0;
}
