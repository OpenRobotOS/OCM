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

/**
 * @brief Abstract base class for sleep mechanisms.
 *
 * The `SleepBase` class defines a common interface for various sleep strategies used within tasks.
 * It allows derived classes to implement specific sleep behaviors based on internal or external timers.
 */
class SleepBase {
 public:
  /**
   * @brief Default constructor.
   */
  SleepBase() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~SleepBase() = default;

  /**
   * @brief Puts the thread to sleep for a specified duration.
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   */
  virtual void Sleep(double duration = 0) = 0;

  /**
   * @brief Sets the sleep period.
   *
   * @param period Period in seconds for the sleep mechanism.
   */
  virtual void SetPeriod(double period) {}

  /**
   * @brief Retrieves the current sleep period.
   *
   * @return The sleep period in seconds.
   */
  virtual double GetPeriod() const { return 0; }

  /**
   * @brief Continues or resumes the sleep mechanism.
   */
  virtual void Continue() = 0;
};

/**
 * @brief Sleep mechanism using an internal timer.
 *
 * The `SleepInternalTimer` class implements the `SleepBase` interface using an internal timing loop.
 * It relies on the `TimerLoop` class to manage sleep intervals.
 */
class SleepInternalTimer : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepInternalTimer` instance.
   *
   * Initializes the internal timer with a default period.
   */
  SleepInternalTimer();

  /**
   * @brief Destructor.
   */
  ~SleepInternalTimer() = default;

  /**
   * @brief Puts the thread to sleep until the next loop iteration.
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Sets the sleep period for the internal timer.
   *
   * @param period Period in seconds.
   */
  void SetPeriod(double period) override;

  /**
   * @brief Retrieves the current sleep period of the internal timer.
   *
   * @return The sleep period in seconds.
   */
  double GetPeriod() const override;

  /**
   * @brief Continues or resets the internal timer clock.
   */
  void Continue() override;

 private:
  TimerLoop timer_loop_; /**< Internal timer loop managing sleep intervals */
};

/**
 * @brief Sleep mechanism using an external timer.
 *
 * The `SleepExternalTimer` class implements the `SleepBase` interface using external synchronization
 * primitives such as semaphores and shared memory. It allows external entities to trigger sleep continuation.
 */
class SleepExternalTimer : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepExternalTimer` instance.
   *
   * Initializes the semaphore and shared memory for external timer synchronization.
   *
   * @param sem_name Name of the semaphore.
   * @param shm_name Name of the shared memory segment.
   */
  SleepExternalTimer(const std::string& sem_name, const std::string& shm_name);

  /**
   * @brief Destructor.
   */
  ~SleepExternalTimer() = default;

  /**
   * @brief Puts the thread to sleep until the external timer signals continuation.
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Sets the sleep period based on the external timer's interval.
   *
   * @param period Period in seconds.
   */
  void SetPeriod(double period) override;

  /**
   * @brief Retrieves the current sleep period based on the external timer's interval.
   *
   * @return The sleep period in seconds.
   */
  double GetPeriod() const override;

  /**
   * @brief Continues or resets the external timer.
   */
  void Continue() override;

 private:
  uint8_t dt_;                     /**< Timer interval delta */
  std::atomic_int interval_time_;  /**< Number of intervals to wait */
  std::atomic_int interval_count_; /**< Counter for intervals waited */
  SharedMemorySemaphore sem_;      /**< Semaphore for external synchronization */
  SharedMemoryData<uint8_t> shm_;  /**< Shared memory segment for timer data */
};

/**
 * @brief Sleep mechanism triggered by a semaphore.
 *
 * The `SleepTrigger` class implements the `SleepBase` interface using a semaphore to control sleep.
 * It allows other processes or threads to signal when the sleep should continue.
 */
class SleepTrigger : public SleepBase {
 public:
  /**
   * @brief Constructs a `SleepTrigger` instance.
   *
   * Initializes the semaphore for sleep synchronization.
   *
   * @param sem_name Name of the semaphore.
   */
  SleepTrigger(const std::string& sem_name);

  /**
   * @brief Destructor.
   */
  ~SleepTrigger() = default;

  /**
   * @brief Puts the thread to sleep until the semaphore is decremented.
   *
   * @param duration Duration in seconds to sleep. Default is 0.
   */
  void Sleep(double duration = 0) override;

  /**
   * @brief Continues or signals the semaphore to allow sleep continuation.
   */
  void Continue() override;

 private:
  SharedMemorySemaphore sem_; /**< Semaphore for sleep synchronization */
};

/**
 * @brief Abstract base class for tasks.
 *
 * The `TaskBase` class provides a common interface and functionality for various tasks.
 * It manages the task's lifecycle, including starting, stopping, and destroying the task thread.
 */
class TaskBase {
 public:
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
  TaskBase(const std::string& thread_name, TimerType type, double sleep_duration, bool all_priority_enable, bool all_cpu_affinity_enable);

  /**
   * @brief Virtual destructor.
   */
  virtual ~TaskBase();

  /**
   * @brief Runs the task.
   *
   * This is a pure virtual function that must be implemented by derived classes to define
   * the task's specific behavior.
   */
  virtual void Run() = 0;

  /**
   * @brief Retrieves the duration of the last run.
   *
   * @return Duration in seconds of the last run.
   */
  double GetRunDuration() const;

  /**
   * @brief Retrieves the duration of the last loop iteration.
   *
   * @return Duration in seconds of the last loop.
   */
  double GetLoopDuration() const;

  /**
   * @brief Sets the period for the task's sleep mechanism.
   *
   * @param period Period in seconds.
   */
  void SetPeriod(double period);

  /**
   * @brief Retrieves the name of the task.
   *
   * @return Name of the task.
   */
  std::string GetTaskName() const;

  /**
   * @brief Retrieves the current state of the task.
   *
   * @return Current `TaskState`.
   */
  TaskState GetState() const;

  /**
   * @brief Starts the task with the specified system settings.
   *
   * @param system_setting System settings for starting the task.
   */
  void TaskStart(const SystemSetting& system_setting);

  /**
   * @brief Stops the task with the specified system settings.
   *
   * @param system_setting System settings for stopping the task.
   */
  void TaskStop(const SystemSetting& system_setting);

  /**
   * @brief Destroys the task, terminating the thread and cleaning up resources.
   */
  void TaskDestroy();

  bool set_rt_flag_; /**< Flag indicating if real-time settings are applied */

 private:
  /**
   * @brief Creates the task thread.
   *
   * Initializes the thread and starts the main loop.
   */
  void TaskCreate();

  /**
   * @brief Main loop of the task thread.
   *
   * Manages the task's execution cycle, including sleeping and running.
   */
  void Loop();

  /**
   * @brief Sets the real-time configuration for the thread.
   *
   * Applies system settings such as priority and CPU affinity.
   *
   * @param system_setting System settings to apply.
   */
  void SetRtConfig(const SystemSetting& system_setting);

  std::atomic_bool thread_alive_;     /**< Flag indicating if the thread is alive */
  std::atomic_bool loop_run_;         /**< Flag indicating if the loop should continue running */
  std::binary_semaphore start_sem_;   /**< Semaphore to signal the start of the task */
  int task_id_;                       /**< Identifier for the task */
  std::string thread_name_;           /**< Name of the task thread */
  std::thread thread_;                /**< Task thread */
  std::atomic<double> run_duration_;  /**< Duration of the last run */
  std::atomic<double> loop_duration_; /**< Duration of the last loop */
  std::atomic_bool destroy_flag_;     /**< Flag indicating if the task should be destroyed */
  std::atomic_bool run_flag_;         /**< Flag indicating if the task should run */

  std::unique_ptr<SleepBase> timer_; /**< Sleep mechanism used by the task */
  double sleep_duration_;            /**< Duration in seconds for the sleep mechanism */

  std::atomic<TaskState> state_;       /**< Current state of the task */
  SystemSetting system_setting_start_; /**< System settings at the start of the task */
  SystemSetting system_setting_stop_;  /**< System settings at the stop of the task */

  bool all_priority_enable_;     /**< Flag to enable all priority settings */
  bool all_cpu_affinity_enable_; /**< Flag to enable all CPU affinity settings */

  std::shared_ptr<spdlog::logger> logger_; /**< Logger for task logging */
};

}  // namespace openrobot::ocm
