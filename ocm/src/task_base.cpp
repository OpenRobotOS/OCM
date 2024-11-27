
#include "task/task_base.hpp"
#include "task/rt/sched_rt.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a `SleepInternalTimer` instance.
 * @brief 构造一个 `SleepInternalTimer` 实例。
 *
 * Initializes the internal timer with a default period of 0.01 seconds.
 * 使用默认的0.01秒周期初始化内部计时器。
 */
SleepInternalTimer::SleepInternalTimer() { SetPeriod(0.01); }

/**
 * @brief Puts the thread to sleep until the next loop iteration.
 * @brief 让线程休眠直到下一个循环迭代。
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 * @param duration 休眠的秒数。默认值为0。
 *
 * This method calls `SleepUntilNextLoop` on the internal `TimerLoop` instance.
 * If the specified duration exceeds the current period, it resets the timer clock.
 * 该方法调用内部 `TimerLoop` 实例的 `SleepUntilNextLoop`。
 * 如果指定的持续时间超过当前周期，则重置定时器时钟。
 */
void SleepInternalTimer::Sleep(double duration) {
  timer_loop_.SleepUntilNextLoop();
  if (duration > GetPeriod()) {
    timer_loop_.ResetClock();
  }
}

/**
 * @brief Sets the sleep period for the internal timer.
 * @brief 设置内部计时器的休眠周期。
 *
 * @param period Period in seconds.
 * @param period 以秒为单位的周期。
 *
 * Delegates the setting of the period to the internal `TimerLoop` instance.
 * 将周期设置委托给内部的 `TimerLoop` 实例。
 */
void SleepInternalTimer::SetPeriod(double period) { timer_loop_.SetPeriod(period); }

/**
 * @brief Retrieves the current sleep period of the internal timer.
 * @brief 获取内部计时器当前的休眠周期。
 *
 * @return The sleep period in seconds.
 * @return 以秒为单位的休眠周期。
 *
 * Retrieves the period from the internal `TimerLoop` instance.
 * 从内部的 `TimerLoop` 实例中获取周期。
 */
double SleepInternalTimer::GetPeriod() const { return timer_loop_.GetPeriod(); }

/**
 * @brief Continues or resets the internal timer clock.
 * @brief 继续或重置内部计时器时钟。
 *
 * Calls `ResetClock` on the internal `TimerLoop` instance.
 * 调用内部 `TimerLoop` 实例的 `ResetClock`。
 */
void SleepInternalTimer::Continue() { timer_loop_.ResetClock(); }

/**
 * @brief Constructs a `SleepExternalTimer` instance.
 * @brief 构造一个 `SleepExternalTimer` 实例。
 *
 * Initializes the semaphore and shared memory for external timer synchronization.
 * Reads the initial timer delta from shared memory.
 * 初始化用于外部定时器同步的信号量和共享内存。
 * 从共享内存中读取初始定时器增量。
 *
 * @param sem_name Name of the semaphore.
 * @param sem_name 信号量的名称。
 * @param shm_name Name of the shared memory segment.
 * @param shm_name 共享内存段的名称。
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
 * @brief 让线程休眠直到外部定时器发出继续信号。
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 * @param duration 休眠的秒数。默认值为0。
 *
 * This method waits on the semaphore and increments the interval count.
 * It continues to sleep until the interval count meets the required interval time.
 * 该方法等待信号量并增加间隔计数。
 * 它会继续休眠，直到间隔计数满足所需的间隔时间。
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
 * @brief 根据外部定时器的间隔设置休眠周期。
 *
 * @param period Period in seconds.
 * @param period 以秒为单位的周期。
 *
 * Calculates the number of intervals required based on the period and timer delta.
 * Ensures that the timer delta is at least 1 millisecond.
 * 根据周期和定时器增量计算所需的间隔数。
 * 确保定时器增量至少为1毫秒。
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
 * @brief 根据外部定时器的间隔获取当前的休眠周期。
 *
 * @return The sleep period in seconds.
 * @return 以秒为单位的休眠周期。
 *
 * Calculates the period by multiplying the interval time by the timer delta.
 * 通过将间隔时间乘以定时器增量来计算周期。
 */
double SleepExternalTimer::GetPeriod() const { return static_cast<double>(interval_time_ * dt_); }

/**
 * @brief Continues or resets the external timer.
 * @brief 继续或重置外部定时器。
 *
 * Sets the interval time to zero and increments the semaphore to release any waiting threads.
 * 将间隔时间设置为零并增加信号量以释放任何等待的线程。
 */
void SleepExternalTimer::Continue() {
  interval_time_.store(0);
  sem_.Increment();
}

/**
 * @brief Constructs a `SleepTrigger` instance.
 * @brief 构造一个 `SleepTrigger` 实例。
 *
 * Initializes the semaphore for sleep synchronization.
 * 初始化用于休眠同步的信号量。
 *
 * @param sem_name Name of the semaphore.
 * @param sem_name 信号量的名称。
 */
SleepTrigger::SleepTrigger(const std::string& sem_name) : sem_(sem_name, 0) {}

/**
 * @brief Puts the thread to sleep until the semaphore is decremented.
 * @brief 让线程休眠直到信号量被递减。
 *
 * @param duration Duration in seconds to sleep. Default is 0.
 * @param duration 休眠的秒数。默认值为0。
 *
 * This method waits on the semaphore to be decremented, effectively putting the thread to sleep
 * until another thread or process signals the semaphore.
 * 该方法等待信号量被递减，有效地将线程置于休眠状态，直到另一个线程或进程发出信号量。
 */
void SleepTrigger::Sleep(double duration) { sem_.Decrement(); }

/**
 * @brief Continues or signals the semaphore to allow sleep continuation.
 * @brief 继续或发出信号量以允许休眠继续。
 *
 * Increments the semaphore, releasing any waiting threads.
 * 增加信号量，释放任何等待的线程。
 */
void SleepTrigger::Continue() { sem_.Increment(); }

/**
 * @brief Constructs a `TaskBase` instance.
 * @brief 构造一个 `TaskBase` 实例。
 *
 * Initializes the task with the specified parameters and creates the task thread.
 * 使用指定的参数初始化任务并创建任务线程。
 *
 * @param thread_name Name of the task thread.
 * @param thread_name 任务线程的名称。
 * @param type Type of timer to use (`TimerType::INTERNAL_TIMER`, `TimerType::EXTERNAL_TIMER`, `TimerType::TRIGGER`).
 * @param type 要使用的定时器类型 (`TimerType::INTERNAL_TIMER`, `TimerType::EXTERNAL_TIMER`, `TimerType::TRIGGER`)。
 * @param sleep_duration Duration in seconds for the sleep mechanism.
 * @param sleep_duration 用于休眠机制的秒数。
 * @param all_priority_enable Flag to enable all priority settings.
 * @param all_priority_enable 启用所有优先级设置的标志。
 * @param all_cpu_affinity_enable Flag to enable all CPU affinity settings.
 * @param all_cpu_affinity_enable 启用所有CPU亲和性设置的标志。
 */
TaskBase::TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable)
    : start_sem_(0), sleep_duration_(sleep_duration), all_priority_enable_(all_priority_enable), all_cpu_affinity_enable_(all_cpu_affinity_enable) {
  logger_ = GetLogger();

  // Initialize the appropriate sleep mechanism based on the timer type
  // 根据定时器类型初始化适当的休眠机制
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
  run_flag_.store(false);
  loop_run_.store(false);
  state_.store(TaskState::INIT);
}

/**
 * @brief Destructor for `TaskBase`.
 * @brief `TaskBase` 的析构函数。
 *
 * Ensures that the task thread is properly destroyed and resources are cleaned up.
 * 确保任务线程被正确销毁并清理资源。
 */
TaskBase::~TaskBase() = default;

/**
 * @brief Creates the task thread.
 * @brief 创建任务线程。
 *
 * Initializes the thread and starts the main loop.
 * 初始化线程并启动主循环。
 */
void TaskBase::TaskCreate() {
  thread_alive_.store(true);
  thread_ = std::thread([this] { Loop(); });
  logger_->debug("[TASK] {} task thread has been created!", thread_name_);
}

/**
 * @brief Starts the task with the specified system settings.
 * @brief 使用指定的系统设置启动任务。
 *
 * @param system_setting System settings for starting the task.
 * @param system_setting 启动任务的系统设置。
 *
 * This method sets the run and loop flags to true, releases the start semaphore,
 * and logs the task start.
 * 该方法将运行和循环标志设置为真，释放启动信号量，并记录任务启动。
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
 * @brief 使用指定的系统设置停止任务。
 *
 * @param system_setting System settings for stopping the task.
 * @param system_setting 停止任务的系统设置。
 *
 * This method sets the run and loop flags to false, signals the timer to continue,
 * and logs the task stop.
 * 该方法将运行和循环标志设置为假，信号定时器继续，并记录任务停止。
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
 * @brief 销毁任务，终止线程并清理资源。
 *
 * This method sets the thread alive flag to false, signals the semaphore,
 * signals the timer to continue, joins the thread if it's joinable, and logs the destruction.
 * 该方法将线程存活标志设置为假，发出信号量，信号定时器继续，如果线程可连接，则连接线程，并记录销毁。
 */
void TaskBase::TaskDestroy() {
  thread_alive_.store(false);
  loop_run_.store(false);
  run_flag_.store(true);
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
 * @brief 任务线程的主循环。
 *
 * Manages the task's execution cycle, including sleeping and running.
 * Applies real-time configurations, handles task states, and measures durations.
 * 管理任务的执行周期，包括休眠和运行。
 * 应用实时配置，处理任务状态，并测量持续时间。
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
    // 进入运行循环前的初始休眠
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int64_t>(sleep_duration_ * 1000)));

    while (loop_run_.load()) {
      timer_->Sleep(GetRunDuration());
      loop_duration_.store(loop_timer.getMs());
      run_timer.start();

      if (run_flag_.load()) {
        Run();
        state_.store(TaskState::RUNNING);
      }

      run_duration_.store(run_timer.getMs());
    }
  }
}

/**
 * @brief Retrieves the duration of the last run.
 * @brief 获取上次运行的持续时间。
 *
 * @return Duration in seconds of the last run.
 * @return 上次运行的持续时间（秒）。
 */
double TaskBase::GetRunDuration() const { return run_duration_.load(); }

/**
 * @brief Retrieves the duration of the last loop iteration.
 * @brief 获取上次循环迭代的持续时间。
 *
 * @return Duration in seconds of the last loop.
 * @return 上次循环的持续时间（秒）。
 */
double TaskBase::GetLoopDuration() const { return loop_duration_.load(); }

/**
 * @brief Sets the period for the task's sleep mechanism.
 * @brief 设置任务休眠机制的周期。
 *
 * @param period Period in seconds.
 * @param period 以秒为单位的周期。
 *
 * Delegates the setting of the period to the sleep mechanism.
 * 将周期设置委托给休眠机制。
 */
void TaskBase::SetPeriod(double period) { timer_->SetPeriod(period); }

/**
 * @brief Retrieves the name of the task.
 * @brief 获取任务的名称。
 *
 * @return Name of the task.
 * @return 任务的名称。
 */
std::string TaskBase::GetTaskName() const { return thread_name_; }

/**
 * @brief Retrieves the current state of the task.
 * @brief 获取任务的当前状态。
 *
 * @return Current `TaskState`.
 * @return 当前的 `TaskState`。
 */
TaskState TaskBase::GetState() const { return state_.load(); }

/**
 * @brief Sets the real-time configuration for the thread.
 * @brief 设置线程的实时配置。
 *
 * @param system_setting System settings to apply.
 * @param system_setting 要应用的系统设置。
 *
 * Applies system settings such as priority and CPU affinity based on the provided `SystemSetting`.
 * 根据提供的 `SystemSetting` 应用优先级和CPU亲和性等系统设置。
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
