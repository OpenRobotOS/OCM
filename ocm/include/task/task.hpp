#pragma once

#include <sys/timerfd.h>
#include <atomic>
#include <thread>
#include "logger/logger.hpp"
#include "ocm/ocm.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {
enum class TaskType : uint8_t {
  TRIGGER = 0,
  TIMER,
};

class TaskBase {
 public:
  TaskBase(const std::string& sem_name, const std::string& thread_name, TaskType type);
  ~TaskBase() = default;

  virtual void Run();
  double GetRunDuration() const;
  double GetLoopDuration() const;
  void SetPeriod(double period);

  void TaskStart();
  void TaskStop();
  void TaskDestroy();

 private:
  void TaskCreate(std::string name);

  void Loop();

  std::atomic_bool thread_alive_;
  std::atomic_bool loop_run_;

  int task_id_;
  std::string name_;
  std::thread thread_;
  std::atomic<double> run_duration_;
  std::atomic<double> loop_duration_;

  TimerLoop timer_loop_;

  TaskType type_;

  SharedMemorySemaphore semaphore_;
  openrobot::ocm::Logger& logger = openrobot::ocm::Logger::getInstance();
};
}  // namespace openrobot::ocm
