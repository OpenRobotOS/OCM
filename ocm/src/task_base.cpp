#include "task/task_base.hpp"
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

SleepExternalTimer::SleepExternalTimer(const std::string& sem_name, const std::string& shm_name)
    : sem_(sem_name, 0), shm_(shm_name, false, sizeof(uint8_t)) {
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
  if (dt_ < 1) {
    dt_ = 1;
  }
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

TaskBase::TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable)
    : start_sem_(0), sleep_duration_(sleep_duration), all_priority_enable_(all_priority_enable), all_cpu_affinity_enable_(all_cpu_affinity_enable) {
  logger_ = spdlog::get("openrobot_ocm_logger");
  if (type == TimerType::INTERNAL_TIMER) {
    timer_ = std::make_unique<SleepInternalTimer>();
  } else if (type == TimerType::EXTERNAL_TIMER) {
    timer_ = std::make_unique<SleepExternalTimer>(thread_name, thread_name);
  } else if (type == TimerType::TRIGGER) {
    timer_ = std::make_unique<SleepTrigger>(thread_name);
  }
  thread_name_ = thread_name;
  TaskCreate();
  run_duration_.store(0.0);
  loop_duration_.store(0.0);
  destroy_flag_.store(false);
  state_.store(TaskState::INIT);
}

TaskBase::~TaskBase() = default;
void TaskBase::TaskCreate() {
  thread_alive_.store(true);
  thread_ = std::thread([this] { Loop(); });
  logger_->debug("[TASK] {} task thread has been created!", thread_name_);
}

void TaskBase::TaskStart(const SystemSetting& system_setting) {
  system_setting_start_ = system_setting;
  run_flag_.store(true);
  loop_run_.store(true);
  start_sem_.release();
  logger_->debug("[TASK] {} task thread ready to run!", thread_name_);
}

void TaskBase::TaskStop(const SystemSetting& system_setting) {
  system_setting_stop_ = system_setting;
  run_flag_.store(false);
  loop_run_.store(false);
  timer_->Continue();
  logger_->debug("[TASK] {} task thread ready to stop!", thread_name_);
}
void TaskBase::TaskDestroy() {
  thread_alive_.store(false);
  loop_run_.store(false);
  destroy_flag_.store(true);
  start_sem_.release();
  timer_->Continue();
  if (thread_.joinable()) {
    thread_.join();
    logger_->debug("[TASK] {} task thread has been safely destroyed!", thread_name_);
  } else {
    logger_->debug("[TASK] {} task thread was already detached or finished!", thread_name_);
  }
}

void TaskBase::Loop() {
  openrobot::ocm::rt::set_thread_name(thread_name_);
  TimerOnce loop_timer;
  TimerOnce run_timer;
  while (thread_alive_.load()) {
    SetRtConfig(system_setting_stop_);
    state_.store(TaskState::STANDBY);
    start_sem_.acquire();
    SetRtConfig(system_setting_start_);
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int64_t>(sleep_duration_ * 1000)));
    while (loop_run_.load()) {
      timer_->Sleep(GetRunDuration());
      loop_duration_.store(loop_timer.getMs());
      run_timer.start();
      if (!destroy_flag_.load() && run_flag_.load()) {
        Run();
        state_.store(TaskState::RUNNING);
      }
      run_duration_.store(run_timer.getMs());
    }
  }
}

double TaskBase::GetRunDuration() const { return run_duration_.load(); }

double TaskBase::GetLoopDuration() const { return loop_duration_.load(); }

void TaskBase::SetPeriod(double period) { timer_->SetPeriod(period); }

std::string TaskBase::GetTaskName() const { return thread_name_; }

TaskState TaskBase::GetState() const { return state_.load(); }

void TaskBase::SetRtConfig(const SystemSetting& system_setting) {
  pid_t pid = gettid();
  if (system_setting.priority != 0 && all_priority_enable_) {
    openrobot::ocm::rt::set_thread_priority(pid, system_setting.priority, SCHED_FIFO);
  }
  if (system_setting.cpu_affinity.size() > 0 && all_cpu_affinity_enable_) {
    openrobot::ocm::rt::set_thread_cpu_affinity(pid, system_setting.cpu_affinity);
  }
}
}  // namespace openrobot::ocm
