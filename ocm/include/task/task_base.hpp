#pragma once

#include <sys/timerfd.h>
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include "common/enum.hpp"
#include "common/struct_type.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "ocm/ocm.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {

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
  SleepExternalTimer(const std::string& sem_name, const std::string& shm_name);
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
  SharedMemoryData<uint8_t> shm_;
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
  TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable);
  virtual ~TaskBase();

  virtual void Run() = 0;
  double GetRunDuration() const;
  double GetLoopDuration() const;
  void SetPeriod(double period);
  std::string GetTaskName() const;
  TaskState GetState() const;
  void TaskStart(const SystemSetting& system_setting);
  void TaskStop(const SystemSetting& system_setting);
  void TaskDestroy();
  bool set_rt_flag_;

 private:
  void TaskCreate();

  void Loop();

  void SetRtConfig(const SystemSetting& system_setting);
  std::atomic_bool thread_alive_;
  std::atomic_bool loop_run_;
  std::binary_semaphore start_sem_;
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
  SystemSetting system_setting_start_;
  SystemSetting system_setting_stop_;

  bool all_priority_enable_;
  bool all_cpu_affinity_enable_;

  std::shared_ptr<spdlog::logger> logger_;
};
}  // namespace openrobot::ocm
