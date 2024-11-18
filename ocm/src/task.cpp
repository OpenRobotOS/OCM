#include "task/task.hpp"
#include "task/rt/sched_rt.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {
TaskBase::TaskBase(const std::string& sem_name, const std::string& thread_name, TaskType type) : semaphore_(sem_name, 0), type_(type) {
  if (type == TaskType::TIMER) {
    timer_loop_.SetPeriod(0.01);
  }
  TaskCreate(thread_name);
  run_duration_.store(0.0);
  loop_duration_.store(0.0);
}
void TaskBase::TaskCreate(std::string name) {
  name_ = name;
  thread_alive_.store(true);
  thread_ = std::thread([this] { Loop(); });
  logger.debug("[TASK] {} task thread has been created!", name_);
}

void TaskBase::TaskStart() {
  loop_run_.store(true);
  logger.debug("[TASK] {} task thread has been started!", name_);
}

void TaskBase::TaskStop() {
  loop_run_.store(false);
  logger.debug("[TASK] {} task thread has been stopped!", name_);
}
void TaskBase::TaskDestroy() {
  thread_alive_.store(false);
  loop_run_.store(false);
  if (semaphore_.GetValue() == 0 && type_ == TaskType::TRIGGER) {
    semaphore_.Increment();
  } else if (type_ == TaskType::TIMER) {
    timer_loop_.ResetClock();
  }
  if (thread_.joinable()) {
    thread_.join();
    logger.debug("[TASK] {} task thread has been safely destroyed!", name_);
  } else {
    logger.debug("[TASK] {} task thread was already detached or finished!", name_);
  }
}

void TaskBase::Run() { logger.debug("[TASK] {} task thread is running!", name_); }

void TaskBase::Loop() {
  openrobot::ocm::rt::set_thread_name(name_);
  TimerOnce loop_timer;
  while (thread_alive_.load()) {
    while (loop_run_.load()) {
      loop_duration_.store(loop_timer.getMs());

      TimerOnce run_timer;
      Run();
      run_duration_.store(run_timer.getMs());

      if (type_ == TaskType::TIMER) {
        timer_loop_.SleepUntilNextLoop();
        if (run_duration_.load() > timer_loop_.GetPeriod()) {
          timer_loop_.ResetClock();
        }
      } else if (type_ == TaskType::TRIGGER) {
        semaphore_.Decrement();
      }
    }
  }
}

double TaskBase::GetRunDuration() const { return run_duration_.load(); }

double TaskBase::GetLoopDuration() const { return loop_duration_.load(); }

void TaskBase::SetPeriod(double period) {
  if (type_ == TaskType::TIMER) {
    timer_loop_.SetPeriod(period);
  } else {
    logger.error("[TASK] SetPeriod is not supported for trigger task type");
  }
}
}  // namespace openrobot::ocm
