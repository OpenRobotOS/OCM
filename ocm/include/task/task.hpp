#pragma once

#include <sys/timerfd.h>
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include "logger/logger.hpp"
#include "ocm/ocm.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {

enum class TaskState : uint8_t { INIT = 0, RUNNING, STANDBY };
enum class TaskType : uint8_t {
  INTERNAL_TIMER = 0,
  EXTERNAL_TIMER,
  TRIGGER,
};

class SleepBase {
 public:
  SleepBase() = default;
  virtual ~SleepBase() = default;
  virtual void Sleep(double duration = 0) = 0;
  virtual void SetPeriod(double period) {}
  virtual double GetPeriod() const { return 0; }
  virtual void Continue() = 0;
};

class SleepInternalTimer : public SleepBase {
 public:
  SleepInternalTimer();
  ~SleepInternalTimer() = default;
  void Sleep(double duration = 0) override;
  void SetPeriod(double period) override;
  double GetPeriod() const override;
  void Continue() override;

 private:
  TimerLoop timer_loop_;
};

class SleepExternalTimer : public SleepBase {
 public:
  SleepExternalTimer();
  ~SleepExternalTimer();
  void Sleep(double duration = 0) override;
  void SetPeriod(double period) override;
  double GetPeriod() const override;
  void Continue() override;

 private:
  uint8_t dt_;
  std::atomic_int interval_time_;
  std::atomic_int interval_count_;
  SharedMemorySemaphore sem_;
  SharedMemoryData shm_;
};

class SleepTrigger : public SleepBase {
 public:
  SleepTrigger(const std::string& sem_name);
  ~SleepTrigger();
  void Sleep(double duration = 0) override;
  void Continue() override;

 private:
  SharedMemorySemaphore sem_;
};

class TaskBase {
 public:
  TaskBase(const std::string& thread_name, TaskType type, double sleep_duration, const std::string& sem_name = "");
  virtual ~TaskBase() = default;

  virtual void Run();
  double GetRunDuration() const;
  double GetLoopDuration() const;
  void SetPeriod(double period);

  void TaskStart();
  void TaskStop();
  void TaskDestroy();

 private:
  void TaskCreate();

  void Loop();

  std::atomic_bool thread_alive_;
  std::atomic_bool loop_run_;
  std::binary_semaphore sem_;
  int task_id_;
  std::string thread_name_;
  std::thread thread_;
  std::atomic<double> run_duration_;
  std::atomic<double> loop_duration_;
  std::atomic_bool destroy_flag_;
  std::atomic_bool run_flag_;

  std::unique_ptr<SleepBase> timer_;
  double sleep_duration_;

  std::atomic<TaskState> state_;

  openrobot::ocm::Logger& logger = openrobot::ocm::Logger::getInstance();
};
}  // namespace openrobot::ocm
