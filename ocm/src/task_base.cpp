#include "task/task_base.hpp"
#include "ocm/ocm.hpp"
#include "task/rt/sched_rt.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a `SleepInternalTimer` instance.
 *
 * Initializes the internal timer with a default period of 0.01 seconds.
 */
SleepInternalTimer::SleepInternalTimer() { SetPeriod(0.01); }

/**
 * @brief Puts the thread to sleep until the next loop iteration.
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 *
 * This method calls `SleepUntilNextLoop` on the internal `TimerLoop` instance.
 * If the specified duration exceeds the current period, it resets the timer clock.
 */
void SleepInternalTimer::Sleep(double duration) {
  timer_loop_.SleepUntilNextLoop();
  if (duration > GetPeriod()) {
    timer_loop_.ResetClock();
  }
}

/**
 * @brief Sets the sleep period for the internal timer.
 *
 * @param period Period in seconds.
 *
 * Delegates the setting of the period to the internal `TimerLoop` instance.
 */
void SleepInternalTimer::SetPeriod(double period) { timer_loop_.SetPeriod(period); }

/**
 * @brief Retrieves the current sleep period of the internal timer.
 *
 * @return The sleep period in seconds.
 *
 * Retrieves the period from the internal `TimerLoop` instance.
 */
double SleepInternalTimer::GetPeriod() const { return timer_loop_.GetPeriod(); }

/**
 * @brief Continues or resets the internal timer clock.
 *
 * Calls `ResetClock` on the internal `TimerLoop` instance.
 */
void SleepInternalTimer::Continue() { timer_loop_.ResetClock(); }

/**
 * @brief Constructs a `SleepExternalTimer` instance.
 *
 * Initializes the semaphore and shared memory for external timer synchronization.
 * Reads the initial timer delta from shared memory.
 *
 * @param sem_name Name of the semaphore.
 * @param shm_name Name of the shared memory segment.
 */
SleepExternalTimer::SleepExternalTimer(const std::string& sem_name, const std::string& shm_name)
    : sem_(sem_name, 0), shm_(shm_name, false, sizeof(uint8_t)) {
  shm_.Lock();
  dt_ = *shm_.Get();
  shm_.UnLock();
  interval_count_.store(0);
}

/**
 * @brief Puts the thread to sleep until the external timer signals continuation.
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 *
 * This method waits on the semaphore and increments the interval count.
 * It continues to sleep until the interval count meets the required interval time.
 */
void SleepExternalTimer::Sleep(double duration) {
  while (interval_count_ < interval_time_ && duration < GetPeriod()) {
    sem_.Decrement();
    interval_count_.fetch_add(1);
  }
  interval_count_.store(0);
}

/**
 * @brief Sets the sleep period based on the external timer's interval.
 *
 * @param period Period in seconds.
 *
 * Calculates the number of intervals required based on the period and timer delta.
 * Ensures that the timer delta is at least 1 millisecond.
 */
void SleepExternalTimer::SetPeriod(double period) {
  if (dt_ < 1) {
    dt_ = 1;
  }
  interval_time_.store(static_cast<int>(period * 1000 / dt_));
  interval_count_.store(0);
}

/**
 * @brief Retrieves the current sleep period based on the external timer's interval.
 *
 * @return The sleep period in seconds.
 *
 * Calculates the period by multiplying the interval time by the timer delta.
 */
double SleepExternalTimer::GetPeriod() const { return static_cast<double>(interval_time_ * dt_); }

/**
 * @brief Continues or resets the external timer.
 *
 * Sets the interval time to zero and increments the semaphore to release any waiting threads.
 */
void SleepExternalTimer::Continue() {
  interval_time_.store(0);
  sem_.Increment();
}

/**
 * @brief Constructs a `SleepTrigger` instance.
 *
 * Initializes the semaphore for sleep synchronization.
 *
 * @param sem_name Name of the semaphore.
 */
SleepTrigger::SleepTrigger(const std::string& sem_name) : sem_(sem_name, 0) {}

/**
 * @brief Puts the thread to sleep until the semaphore is decremented.
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 *
 * This method waits on the semaphore to be decremented, effectively putting the thread to sleep
 * until another thread or process signals the semaphore.
 */
void SleepTrigger::Sleep(double duration) { sem_.Decrement(); }

/**
 * @brief Continues or signals the semaphore to allow sleep continuation.
 *
 * Increments the semaphore, releasing any waiting threads.
 */
void SleepTrigger::Continue() { sem_.Increment(); }

/**
 * @brief Constructs a `TaskBase` instance.
 *
 * Initializes the task with the specified parameters and creates the task thread.
 *
 * @param thread_name Name of the task thread.
 * @param type Type of timer to use (`TimerType::INTERNAL_TIMER`, `TimerType::EXTERNAL_TIMER`, `TimerType::TRIGGER`).
 * @param sleep_duration Duration in seconds for the sleep mechanism.
 * @param all_priority_enable Flag to enable all priority settings.
 * @param all_cpu_affinity_enable Flag to enable all CPU affinity settings.
 */
TaskBase::TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable)
    : start_sem_(0), sleep_duration_(sleep_duration), all_priority_enable_(all_priority_enable), all_cpu_affinity_enable_(all_cpu_affinity_enable) {
  logger_ = GetLogger();

  // Initialize the appropriate sleep mechanism based on the timer type
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

/**
 * @brief Destructor for `TaskBase`.
 *
 * Ensures that the task thread is properly destroyed and resources are cleaned up.
 */
TaskBase::~TaskBase() = default;

/**
 * @brief Creates the task thread.
 *
 * Initializes the thread and starts the main loop.
 */
void TaskBase::TaskCreate() {
  thread_alive_.store(true);
  thread_ = std::thread([this] { Loop(); });
  logger_->debug("[TASK] {} task thread has been created!", thread_name_);
}

/**
 * @brief Starts the task with the specified system settings.
 *
 * @param system_setting System settings for starting the task.
 *
 * This method sets the run and loop flags to true, releases the start semaphore,
 * and logs the task start.
 */
void TaskBase::TaskStart(const SystemSetting& system_setting) {
  system_setting_start_ = system_setting;
  run_flag_.store(true);
  loop_run_.store(true);
  start_sem_.release();
  logger_->debug("[TASK] {} task thread ready to run!", thread_name_);
}

/**
 * @brief Stops the task with the specified system settings.
 *
 * @param system_setting System settings for stopping the task.
 *
 * This method sets the run and loop flags to false, signals the timer to continue,
 * and logs the task stop.
 */
void TaskBase::TaskStop(const SystemSetting& system_setting) {
  system_setting_stop_ = system_setting;
  run_flag_.store(false);
  loop_run_.store(false);
  timer_->Continue();
  logger_->debug("[TASK] {} task thread ready to stop!", thread_name_);
}

/**
 * @brief Destroys the task, terminating the thread and cleaning up resources.
 *
 * This method sets the thread alive flag to false, signals the semaphore,
 * signals the timer to continue, joins the thread if it's joinable, and logs the destruction.
 */
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

/**
 * @brief Main loop of the task thread.
 *
 * Manages the task's execution cycle, including sleeping and running.
 * Applies real-time configurations, handles task states, and measures durations.
 */
void TaskBase::Loop() {
  openrobot::ocm::rt::set_thread_name(thread_name_);
  TimerOnce loop_timer;
  TimerOnce run_timer;

  while (thread_alive_.load()) {
    SetRtConfig(system_setting_stop_);
    state_.store(TaskState::STANDBY);
    start_sem_.acquire();
    SetRtConfig(system_setting_start_);

    // Initial sleep before entering the run loop
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

/**
 * @brief Retrieves the duration of the last run.
 *
 * @return Duration in seconds of the last run.
 */
double TaskBase::GetRunDuration() const { return run_duration_.load(); }

/**
 * @brief Retrieves the duration of the last loop iteration.
 *
 * @return Duration in seconds of the last loop.
 */
double TaskBase::GetLoopDuration() const { return loop_duration_.load(); }

/**
 * @brief Sets the period for the task's sleep mechanism.
 *
 * @param period Period in seconds.
 *
 * Delegates the setting of the period to the sleep mechanism.
 */
void TaskBase::SetPeriod(double period) { timer_->SetPeriod(period); }

/**
 * @brief Retrieves the name of the task.
 *
 * @return Name of the task.
 */
std::string TaskBase::GetTaskName() const { return thread_name_; }

/**
 * @brief Retrieves the current state of the task.
 *
 * @return Current `TaskState`.
 */
TaskState TaskBase::GetState() const { return state_.load(); }

/**
 * @brief Sets the real-time configuration for the thread.
 *
 * Applies system settings such as priority and CPU affinity based on the provided `SystemSetting`.
 *
 * @param system_setting System settings to apply.
 */
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
