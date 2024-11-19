#include <format>
#include "task/task.hpp"

class Task : public openrobot::ocm::TaskBase {
 public:
  Task() : openrobot::ocm::TaskBase("openrobot_task", openrobot::ocm::TaskType::EXTERNAL_TIMER, 0.0, "openrobot_task") {}
  void Run() override { std::cout << std::format("[openrobot_task]{}", this->GetLoopDuration()) << std::endl; }
};

class TaskTimer : public openrobot::ocm::TaskBase {
 public:
  TaskTimer()
      : openrobot::ocm::TaskBase("openrobot_task_timer", openrobot::ocm::TaskType::INTERNAL_TIMER, 0.0),
        sem_("OPENROBOT_OCM_SEM_TIMING_GENERATOR", 0),
        shm_("OPENROBOT_OCM_SHM_TIMING_GENERATOR_DT", sizeof(uint8_t)) {
    shm_.Lock();
    *shm_.Get() = 1;  // ms
    shm_.UnLock();
  }
  void Run() override { sem_.IncrementWhenZero(); }

 private:
  openrobot::ocm::SharedMemorySemaphore sem_;
  openrobot::ocm::SharedMemoryData shm_;
};

int main() {
  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);
  timer_task.TaskStart();

  Task task;
  task.SetPeriod(0.001);
  task.TaskStart();

  std::this_thread::sleep_for(std::chrono::seconds(5));
  timer_task.TaskDestroy();
  task.TaskDestroy();
  return 0;
}
