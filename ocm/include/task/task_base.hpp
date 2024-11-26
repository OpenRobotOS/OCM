
#pragma once

#include <sys/timerfd.h>
#include <atomic>
#include <cstdint>
#include <semaphore>
#include <thread>
#include "common/enum.hpp"
#include "common/struct_type.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "ocm/shard_memory_data.hpp"
#include "ocm/shared_memory_semaphore.hpp"
#include "task/timer.hpp"

namespace openrobot::ocm {

/**
 * @brief Abstract base class for sleep mechanisms.
 *        抽象的睡眠机制基类。
 *
 * The `SleepBase` class defines a common interface for various sleep strategies used within tasks.
 * `SleepBase`类为任务中使用的各种睡眠策略定义了一个通用接口。
 *
 * It allows derived classes to implement specific sleep behaviors based on internal or external timers.
 * 它允许派生类根据内部或外部定时器实现特定的睡眠行为。
 */
class SleepBase {
 public:
  /**
   * @brief Default constructor.
   *        默认构造函数。
   */
  SleepBase() = default;

  /**
   * @brief Virtual destructor.
   *        虚析构函数。
   */
  virtual ~SleepBase() = default;

  /**
   * @brief Puts the thread to sleep for a specified duration.
   *        使线程在指定的持续时间内进入睡眠。
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   *                睡眠的持续时间，以秒为单位。默认为0。
   */
  virtual void Sleep(double duration = 0) = 0;

  /**
   * @brief Sets the sleep period.
   *        设置睡眠周期。
   *
   * @param period Period in seconds for the sleep mechanism.
   *               睡眠机制的周期，以秒为单位。
   */
  virtual void SetPeriod(double period) {}

  /**
   * @brief Retrieves the current sleep period.
   *        获取当前的睡眠周期。
   *
   * @return The sleep period in seconds.
   *         睡眠周期，以秒为单位。
   */
  virtual double GetPeriod() const { return 0; }

  /**
   * @brief Continues or resumes the sleep mechanism.
   *        继续或恢复睡眠机制。
   */
  virtual void Continue() = 0;
};

/**
 * @brief Sleep mechanism using an internal timer.
 *        使用内部定时器的睡眠机制。
 *
 * The `SleepInternalTimer` class implements the `SleepBase` interface using an internal timing loop.
 * `SleepInternalTimer`类使用内部时间循环实现了`SleepBase`接口。
 *
 * It relies on the `TimerLoop` class to manage sleep intervals.
 * 它依赖于`TimerLoop`类来管理睡眠间隔。
 */
class SleepInternalTimer : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepInternalTimer` instance.
   *        构造一个`SleepInternalTimer`实例。
   *
   * Initializes the internal timer with a default period.
   * 初始化内部定时器，使用默认周期。
   */
  SleepInternalTimer();

  /**
   * @brief Destructor.
   *        析构函数。
   */
  ~SleepInternalTimer() = default;

  /**
   * @brief Puts the thread to sleep until the next loop iteration.
   *        使线程睡眠直到下一个循环迭代。
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   *                睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Sets the sleep period for the internal timer.
   *        设置内部定时器的睡眠周期。
   *
   * @param period Period in seconds.
   *               周期，以秒为单位。
   */
  void SetPeriod(double period) override;

  /**
   * @brief Retrieves the current sleep period of the internal timer.
   *        获取内部定时器的当前睡眠周期。
   *
   * @return The sleep period in seconds.
   *         睡眠周期，以秒为单位。
   */
  double GetPeriod() const override;

  /**
   * @brief Continues or resets the internal timer clock.
   *        继续或重置内部定时器时钟。
   */
  void Continue() override;

 private:
  TimerLoop timer_loop_; /**< Internal timer loop managing sleep intervals
                           内部定时器循环，用于管理睡眠间隔 */
};

/**
 * @brief Sleep mechanism using an external timer.
 *        使用外部定时器的睡眠机制。
 *
 * The `SleepExternalTimer` class implements the `SleepBase` interface using external synchronization
 * primitives such as semaphores and shared memory. It allows external entities to trigger sleep continuation.
 * `SleepExternalTimer`类使用信号量和共享内存等外部同步原语实现了`SleepBase`接口。
 * 它允许外部实体触发睡眠的继续。
 */
class SleepExternalTimer : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepExternalTimer` instance.
   *        构造一个`SleepExternalTimer`实例。
   *
   * Initializes the semaphore and shared memory for external timer synchronization.
   * 初始化用于外部定时器同步的信号量和共享内存。
   *
   * @param sem_name Name of the semaphore.
   *                 信号量名称。
   * @param shm_name Name of the shared memory segment.
   *                 共享内存段名称。
   */
  SleepExternalTimer(const std::string& sem_name, const std::string& shm_name);

  /**
   * @brief Destructor.
   *        析构函数。
   */
  ~SleepExternalTimer() = default;

  /**
   * @brief Puts the thread to sleep until the external timer signals continuation.
   *        使线程睡眠，直到外部定时器发出继续信号。
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   *                睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Sets the sleep period based on the external timer's interval.
   *        根据外部定时器的间隔设置睡眠周期。
   *
   * @param period Period in seconds.
   *               周期，以秒为单位。
   */
  void SetPeriod(double period) override;

  /**
   * @brief Retrieves the current sleep period based on the external timer's interval.
   *        获取基于外部定时器间隔的当前睡眠周期。
   *
   * @return The sleep period in seconds.
   *         睡眠周期，以秒为单位。
   */
  double GetPeriod() const override;

  /**
   * @brief Continues or resets the external timer.
   *        继续或重置外部定时器。
   */
  void Continue() override;

 private:
  uint8_t dt_;                     /**< Timer interval delta 定时器间隔增量 */
  std::atomic_int interval_time_;  /**< Number of intervals to wait 等待的间隔数量 */
  std::atomic_int interval_count_; /**< Counter for intervals waited 已等待间隔的计数器 */
  SharedMemorySemaphore sem_;      /**< Semaphore for external synchronization 用于外部同步的信号量 */
  SharedMemoryData<uint8_t> shm_;  /**< Shared memory segment for timer data 定时器数据的共享内存段 */
};

/**
 * @brief Sleep mechanism triggered by a semaphore.
 *        由信号量触发的睡眠机制。
 *
 * The `SleepTrigger` class implements the `SleepBase` interface using a semaphore to control sleep.
 * It allows other processes or threads to signal when the sleep should continue.
 * `SleepTrigger`类使用信号量控制睡眠，实现在`SleepBase`接口。
 * 它允许其他进程或线程发出信号，指示何时应继续睡眠。
 */
class SleepTrigger : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepTrigger` instance.
   *        构造一个`SleepTrigger`实例。
   *
   * Initializes the semaphore for sleep synchronization.
   * 初始化用于睡眠同步的信号量。
   *
   * @param sem_name Name of the semaphore.
   *                 信号量名称。
   */
  SleepTrigger(const std::string& sem_name);

  /**
   * @brief Destructor.
   *        析构函数。
   */
  ~SleepTrigger() = default;

  /**
   * @brief Puts the thread to sleep until the semaphore is decremented.
   *        使线程睡眠，直到信号量被递减。
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   *                睡眠的持续时间，以秒为单位。默认为0。
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Continues or signals the semaphore to allow sleep continuation.
   *        继续或发送信号量信号，允许睡眠继续。
   */
  void Continue() override;

 private:
  SharedMemorySemaphore sem_; /**< Semaphore for sleep synchronization 用于睡眠同步的信号量 */
};

/**
 * @brief Abstract base class for tasks.
 *        抽象的任务基类。
 *
 * The `TaskBase` class provides a common interface and functionality for various tasks.
 * 它为各种任务提供了通用的接口和功能。
 *
 * It manages the task's lifecycle, including starting, stopping, and destroying the task thread.
 * 它管理任务的生命周期，包括启动、停止和销毁任务线程。
 */
class TaskBase {
 public:
  /**
   * @brief Constructs a `TaskBase` instance.
   *        构造一个`TaskBase`实例。
   *
   * Initializes the task with the specified parameters and creates the task thread.
   * 使用指定的参数初始化任务并创建任务线程。
   *
   * @param thread_name Name of the task thread.
   *                    任务线程名称。
   * @param type Type of timer to use (`TimerType::INTERNAL_TIMER`, `TimerType::EXTERNAL_TIMER`, `TimerType::TRIGGER`).
   *             要使用的定时器类型（`TimerType::INTERNAL_TIMER`，`TimerType::EXTERNAL_TIMER`，`TimerType::TRIGGER`）。
   * @param sleep_duration Duration in seconds for the sleep mechanism.
   *                        睡眠机制的持续时间，以秒为单位。
   * @param all_priority_enable Flag to enable all priority settings.
   *                             启用所有优先级设置的标志。
   * @param all_cpu_affinity_enable Flag to enable all CPU affinity settings.
   *                                 启用所有CPU亲和性设置的标志。
   */
  TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable);

  /**
   * @brief Virtual destructor.
   *        虚析构函数。
   */
  virtual ~TaskBase();

  /**
   * @brief Runs the task.
   *        运行任务。
   *
   * This is a pure virtual function that must be implemented by derived classes to define
   * the task's specific behavior.
   * 这是一个纯虚函数，必须由派生类实现以定义任务的特定行为。
   */
  virtual void Run() = 0;

  /**
   * @brief Retrieves the duration of the last run.
   *        获取上次运行的持续时间。
   *
   * @return Duration in seconds of the last run.
   *         上次运行的持续时间，以秒为单位。
   */
  double GetRunDuration() const;

  /**
   * @brief Retrieves the duration of the last loop iteration.
   *        获取上次循环迭代的持续时间。
   *
   * @return Duration in seconds of the last loop.
   *         上次循环的持续时间，以秒为单位。
   */
  double GetLoopDuration() const;

  /**
   * @brief Sets the period for the task's sleep mechanism.
   *        设置任务睡眠机制的周期。
   *
   * @param period Period in seconds.
   *               周期，以秒为单位。
   */
  void SetPeriod(double period);

  /**
   * @brief Retrieves the name of the task.
   *        获取任务的名称。
   *
   * @return Name of the task.
   *         任务名称。
   */
  std::string GetTaskName() const;

  /**
   * @brief Retrieves the current state of the task.
   *        获取任务的当前状态。
   *
   * @return Current `TaskState`.
   *         当前的`TaskState`。
   */
  TaskState GetState() const;

  /**
   * @brief Starts the task with the specified system settings.
   *        使用指定的系统设置启动任务。
   *
   * @param system_setting System settings for starting the task.
   *                        启动任务的系统设置。
   */
  void TaskStart(const SystemSetting& system_setting);

  /**
   * @brief Stops the task with the specified system settings.
   *        使用指定的系统设置停止任务。
   *
   * @param system_setting System settings for stopping the task.
   *                        停止任务的系统设置。
   */
  void TaskStop(const SystemSetting& system_setting);

  /**
   * @brief Destroys the task, terminating the thread and cleaning up resources.
   *        销毁任务，终止线程并清理资源。
   */
  void TaskDestroy();

  bool set_rt_flag_; /**< Flag indicating if real-time settings are applied
                      标志，指示是否应用了实时设置 */

 private:
  /**
   * @brief Creates the task thread.
   *        创建任务线程。
   *
   * Initializes the thread and starts the main loop.
   * 初始化线程并启动主循环。
   */
  void TaskCreate();

  /**
   * @brief Main loop of the task thread.
   *        任务线程的主循环。
   *
   * Manages the task's execution cycle, including sleeping and running.
   * 管理任务的执行周期，包括睡眠和运行。
   */
  void Loop();

  /**
   * @brief Sets the real-time configuration for the thread.
   *        设置线程的实时配置。
   *
   * Applies system settings such as priority and CPU affinity.
   * 应用诸如优先级和CPU亲和性等系统设置。
   *
   * @param system_setting System settings to apply.
   *                        要应用的系统设置。
   */
  void SetRtConfig(const SystemSetting& system_setting);

  std::atomic_bool thread_alive_;     /**< Flag indicating if the thread is alive
                                       标志，指示线程是否存活 */
  std::atomic_bool loop_run_;         /**< Flag indicating if the loop should continue running
                                       标志，指示循环是否应继续运行 */
  std::binary_semaphore start_sem_;   /**< Semaphore to signal the start of the task
                                       用于信号任务开始的信号量 */
  int task_id_;                       /**< Identifier for the task
                                       任务的标识符 */
  std::string thread_name_;           /**< Name of the task thread
                                       任务线程的名称 */
  std::thread thread_;                /**< Task thread
                                       任务线程 */
  std::atomic<double> run_duration_;  /**< Duration of the last run
                                       上次运行的持续时间 */
  std::atomic<double> loop_duration_; /**< Duration of the last loop
                                       上次循环的持续时间 */
  std::atomic_bool destroy_flag_;     /**< Flag indicating if the task should be destroyed
                                       标志，指示任务是否应被销毁 */
  std::atomic_bool run_flag_;         /**< Flag indicating if the task should run
                                       标志，指示任务是否应运行 */

  std::unique_ptr<SleepBase> timer_; /**< Sleep mechanism used by the task
                                       任务使用的睡眠机制 */
  double sleep_duration_;            /**< Duration in seconds for the sleep mechanism
                                       睡眠机制的持续时间，以秒为单位 */

  std::atomic<TaskState> state_;       /**< Current state of the task
                                       任务的当前状态 */
  SystemSetting system_setting_start_; /**< System settings at the start of the task
                                       任务开始时的系统设置 */
  SystemSetting system_setting_stop_;  /**< System settings at the stop of the task
                                       任务停止时的系统设置 */

  bool all_priority_enable_;     /**< Flag to enable all priority settings
                                   启用所有优先级设置的标志 */
  bool all_cpu_affinity_enable_; /**< Flag to enable all CPU affinity settings
                                   启用所有CPU亲和性设置的标志 */

  std::shared_ptr<spdlog::logger> logger_; /**< Logger for task logging
                                              任务日志记录的记录器 */
};

}  // namespace openrobot::ocm
