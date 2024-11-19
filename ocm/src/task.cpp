#include "task/task.hpp"
#include "ocm/ocm.hpp"
#include "task/rt/sched_rt.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {
SleepInternalTimer::SleepInternalTimer() { SetPeriod(0.01); }

void SleepInternalTimer::Sleep(double duration) {
  timer_loop_.SleepUntilNextLoop();
  if (duration > GetPeriod()) {
    timer_loop_.ResetClock();
  }
}
void SleepInternalTimer::SetPeriod(double period) { timer_loop_.SetPeriod(period); }
double SleepInternalTimer::GetPeriod() const { return timer_loop_.GetPeriod(); }
void SleepInternalTimer::Continue() { timer_loop_.ResetClock(); }

SleepExternalTimer::SleepExternalTimer()
    : sem_("OPENROBOT_OCM_SEM_TIMING_GENERATOR", 0), shm_("OPENROBOT_OCM_SHM_TIMING_GENERATOR_DT", sizeof(uint8_t)) {
  shm_.Lock();
  dt_ = *shm_.Get();
  shm_.UnLock();
  interval_count_.store(0);
}
SleepExternalTimer::~SleepExternalTimer() {
  sem_.Destroy();
  shm_.CloseExisting();
}
void SleepExternalTimer::Sleep(double duration) {
  while (interval_count_ < interval_time_ && duration < GetPeriod()) {
    sem_.Decrement();
    interval_count_.fetch_add(1);
  }
  interval_count_.store(0);
}
void SleepExternalTimer::SetPeriod(double period) {
  interval_time_.store(static_cast<int>(period * 1000 / dt_));
  interval_count_.store(0);
}
double SleepExternalTimer::GetPeriod() const { return static_cast<double>(interval_time_ * dt_); }
void SleepExternalTimer::Continue() {
  interval_time_.store(0);
  sem_.Increment();
}

SleepTrigger::SleepTrigger(const std::string& sem_name) : sem_(sem_name, 0) {}
SleepTrigger::~SleepTrigger() { sem_.Destroy(); }
void SleepTrigger::Sleep(double duration) { sem_.Decrement(); }
void SleepTrigger::Continue() { sem_.Increment(); }

TaskBase::TaskBase(const std::string& thread_name, TaskType type, double sleep_duration, const std::string& sem_name)
    : sem_(0), sleep_duration_(sleep_duration) {
  if (type == TaskType::INTERNAL_TIMER) {
    timer_ = std::make_unique<SleepInternalTimer>();
  } else if (type == TaskType::EXTERNAL_TIMER) {
    timer_ = std::make_unique<SleepExternalTimer>();
  } else if (type == TaskType::TRIGGER) {
    timer_ = std::make_unique<SleepTrigger>(sem_name);
  }
  thread_name_ = thread_name;
  TaskCreate();
  run_duration_.store(0.0);
  loop_duration_.store(0.0);
  destroy_flag_.store(false);
  state_.store(TaskState::INIT);
}

void TaskBase::TaskCreate() {
  thread_alive_.store(true);
  thread_ = std::thread([this] { Loop(); });
  logger.debug("[TASK] {} task thread has been created!", thread_name_);
}

void TaskBase::TaskStart() {
  run_flag_.store(true);
  loop_run_.store(true);
  sem_.release();
  state_.store(TaskState::RUNNING);
  logger.debug("[TASK] {} task thread has been started!", thread_name_);
}

void TaskBase::TaskStop() {
  run_flag_.store(false);
  loop_run_.store(false);
  timer_->Continue();
  logger.debug("[TASK] {} task thread has been stopped!", thread_name_);
}
void TaskBase::TaskDestroy() {
  thread_alive_.store(false);
  loop_run_.store(false);
  destroy_flag_.store(true);
  timer_->Continue();
  if (thread_.joinable()) {
    thread_.join();
    logger.debug("[TASK] {} task thread has been safely destroyed!", thread_name_);
  } else {
    logger.debug("[TASK] {} task thread was already detached or finished!", thread_name_);
  }
}

void TaskBase::Run() { logger.debug("[TASK] {} task thread is running!", thread_name_); }

void TaskBase::Loop() {
  std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int64_t>(sleep_duration_ * 1000)));
  openrobot::ocm::rt::set_thread_name(thread_name_);
  TimerOnce loop_timer;
  TimerOnce run_timer;
  while (thread_alive_.load()) {
    state_.store(TaskState::STANDBY);
    sem_.acquire();
    while (loop_run_.load()) {
      timer_->Sleep(GetRunDuration());
      loop_duration_.store(loop_timer.getMs());
      run_timer.start();
      if (!destroy_flag_.load() && run_flag_.load()) {
        Run();
      }
      run_duration_.store(run_timer.getMs());
    }
  }
}

double TaskBase::GetRunDuration() const { return run_duration_.load(); }

double TaskBase::GetLoopDuration() const { return loop_duration_.load(); }

void TaskBase::SetPeriod(double period) { timer_->SetPeriod(period); }
}  // namespace openrobot::ocm
