#include <spdlog/common.h>
#include "logger/logger.hpp"
#include "ocm/ocm.hpp"
#include "task/task.hpp"

class TestTimerTask : public openrobot::ocm::TaskBase {
 public:
  TestTimerTask()
      : openrobot::ocm::TaskBase("openrobot_trigger_task", "openrobot_timer_task", openrobot::ocm::TaskType::TIMER),
        semaphore_("openrobot_trigger_task", 0) {}
  void Run() override {
    if (semaphore_.GetValue() == 0) {
      semaphore_.Increment();
    }
  }

  openrobot::ocm::SharedMemorySemaphore semaphore_;
  openrobot::ocm::Logger& logger = openrobot::ocm::Logger::getInstance();
};

class TestTriggerTask : public openrobot::ocm::TaskBase {
 public:
  TestTriggerTask() : openrobot::ocm::TaskBase("openrobot_trigger_task", "openrobot_trigger_task", openrobot::ocm::TaskType::TRIGGER) {}
  void Run() override { logger.debug("[openrobot_trigger_task]{}", this->GetLoopDuration()); }
  openrobot::ocm::Logger& logger = openrobot::ocm::Logger::getInstance();
};

int main() {
  auto& logger = openrobot::ocm::Logger::getInstance();
  logger.setLogLevel(spdlog::level::debug);

  TestTimerTask timer_task;
  TestTriggerTask trigger_task;
  timer_task.SetPeriod(0.001);
  timer_task.TaskStart();
  trigger_task.TaskStart();

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
