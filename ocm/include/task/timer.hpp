// timer.hpp
#pragma once

#include <stdint.h>
#include <time.h>

/*!
 * @file timer.hpp
 * @brief Contains declarations for TimerOnce and TimerLoop classes used for time measurement and loop management.
 */

namespace openrobot::ocm {

/**
 * @class TimerOnce
 * @brief Measures elapsed time using the CLOCK_MONOTONIC clock.
 *
 * The `TimerOnce` class provides functionality to measure time intervals in milliseconds, nanoseconds, and seconds.
 * It also allows retrieving the current time in milliseconds.
 */
class TimerOnce {
 public:
  /**
   * @brief Constructs and starts the timer.
   *
   * Initializes the timer by capturing the current time.
   */
  explicit TimerOnce();

  /**
   * @brief Starts or restarts the timer.
   *
   * Captures the current time as the start time.
   */
  void start();

  /**
   * @brief Retrieves the elapsed time in milliseconds since the last start or getNs call.
   *
   * @return Elapsed time in milliseconds as a double.
   */
  double getMs();

  /**
   * @brief Retrieves the elapsed time in nanoseconds since the last start or getNs call.
   *
   * @return Elapsed time in nanoseconds as a 64-bit integer.
   */
  int64_t getNs();

  /**
   * @brief Retrieves the elapsed time in seconds since the last start or getNs call.
   *
   * @return Elapsed time in seconds as a double.
   */
  double getSeconds();

  /**
   * @brief Retrieves the current time in milliseconds since the epoch.
   *
   * @return Current time in milliseconds as a double.
   */
  double getNowTime() const;

 private:
  struct timespec _startTime; /**< Stores the start time for elapsed time calculations */
};

/**
 * @class TimerLoop
 * @brief Manages loop timing using absolute wake-up times.
 *
 * The `TimerLoop` class provides functionality to set loop periods and sleep until the next loop iteration.
 * It ensures that loops run at consistent intervals based on the specified period.
 */
class TimerLoop {
 public:
  /**
   * @brief Resets the internal clock to the current time.
   *
   * Captures the current time as the wake-up absolute time and resets internal counters.
   */
  void ResetClock();

  /**
   * @brief Sets the loop period and initializes the wake-up time.
   *
   * @param period Period of the loop in seconds.
   *
   * This method calculates the period in milliseconds and nanoseconds, resets the internal clock,
   * and schedules the first wake-up time.
   */
  void SetPeriod(double period);

  /**
   * @brief Retrieves the current loop period.
   *
   * @return Loop period in milliseconds as a double.
   */
  double GetPeriod() const;

  /**
   * @brief Sleeps until the next loop iteration based on the wake-up absolute time.
   *
   * This method puts the thread to sleep until the scheduled wake-up time and then schedules the next wake-up.
   */
  void SleepUntilNextLoop();

 private:
  /**
   * @brief Adds the loop period to the current wake-up time, handling nanosecond overflow.
   *
   * Updates the wake-up absolute time by adding the loop period, ensuring that nanoseconds do not exceed
   * one billion (the number of nanoseconds in a second).
   */
  void AddPeriod();

  timespec wake_abs_time_; /**< Absolute wake-up time for the next loop iteration */
  long time_ns_;           /**< Current nanosecond component of the wake-up time */
  long time_s_;            /**< Current second component of the wake-up time */
  double period_ms_;       /**< Loop period in milliseconds */
  long period_ns_;         /**< Loop period in nanoseconds */

  // Constants for nanosecond calculations
  static constexpr long NS_CARRY = 999999999; /**< Nanosecond carry-over threshold */
  static constexpr long NS_TO_S = 1000000000; /**< Number of nanoseconds in one second */
};

}  // namespace openrobot::ocm
