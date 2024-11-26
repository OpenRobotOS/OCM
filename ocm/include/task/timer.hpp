// Start of Selection
#pragma once

#include <stdint.h>
#include <time.h>

/*!
 * @file timer.hpp
 * @brief Contains declarations for TimerOnce and TimerLoop classes used for time measurement and loop management.
 * @brief 包含用于时间测量和循环管理的TimerOnce和TimerLoop类的声明。
 */

namespace openrobot::ocm {

/**
 * @class TimerOnce
 * @brief Measures elapsed time using the CLOCK_MONOTONIC clock.
 * @brief 使用CLOCK_MONOTONIC时钟测量经过的时间。
 *
 * The `TimerOnce` class provides functionality to measure time intervals in milliseconds, nanoseconds, and seconds.
 * It also allows retrieving the current time in milliseconds.
 * @brief `TimerOnce`类提供了以毫秒、纳秒和秒为单位测量时间间隔的功能。
 * 它还允许以毫秒为单位获取当前时间。
 */
class TimerOnce {
 public:
  /**
   * @brief Constructs and starts the timer.
   * @brief 构造并启动定时器。
   *
   * Initializes the timer by capturing the current time.
   * @brief 通过获取当前时间来初始化定时器。
   */
  explicit TimerOnce();

  /**
   * @brief Starts or restarts the timer.
   * @brief 启动或重启定时器。
   *
   * Captures the current time as the start time.
   * @brief 将当前时间捕捉为开始时间。
   */
  void start();

  /**
   * @brief Retrieves the elapsed time in milliseconds since the last start or getNs call.
   * @brief 获取自上次启动或调用getNs以来经过的毫秒数。
   *
   * @return Elapsed time in milliseconds as a double.
   * @return 以双精度浮点数表示的经过毫秒数。
   */
  double getMs();

  /**
   * @brief Retrieves the elapsed time in nanoseconds since the last start or getNs call.
   * @brief 获取自上次启动或调用getNs以来经过的纳秒数。
   *
   * @return Elapsed time in nanoseconds as a 64-bit integer.
   * @return 以64位整数表示的经过纳秒数。
   */
  int64_t getNs();

  /**
   * @brief Retrieves the elapsed time in seconds since the last start or getNs call.
   * @brief 获取自上次启动或调用getNs以来经过的秒数。
   *
   * @return Elapsed time in seconds as a double.
   * @return 以双精度浮点数表示的经过秒数。
   */
  double getSeconds();

  /**
   * @brief Retrieves the current time in milliseconds since the epoch.
   * @brief 获取自纪元以来的当前时间，以毫秒为单位。
   *
   * @return Current time in milliseconds as a double.
   * @return 以双精度浮点数表示的当前时间（毫秒）。
   */
  double getNowTime() const;

 private:
  struct timespec _startTime; /**< Stores the start time for elapsed time calculations */
  /**< 存储用于计算经过时间的开始时间 */
};

/**
 * @class TimerLoop
 * @brief Manages loop timing using absolute wake-up times.
 * @brief 使用绝对唤醒时间管理循环定时。
 *
 * The `TimerLoop` class provides functionality to set loop periods and sleep until the next loop iteration.
 * It ensures that loops run at consistent intervals based on the specified period.
 * @brief `TimerLoop`类提供了设置循环周期和休眠直到下一个循环迭代的功能。
 * 它确保循环根据指定的周期以一致的间隔运行。
 */
class TimerLoop {
 public:
  /**
   * @brief Resets the internal clock to the current time.
   * @brief 将内部时钟重置为当前时间。
   *
   * Captures the current time as the wake-up absolute time and resets internal counters.
   * @brief 将当前时间捕捉为唤醒绝对时间并重置内部计数器。
   */
  void ResetClock();

  /**
   * @brief Sets the loop period and initializes the wake-up time.
   * @brief 设置循环周期并初始化唤醒时间。
   *
   * @param period Period of the loop in seconds.
   * @param period 循环的周期（秒）。
   *
   * This method calculates the period in milliseconds and nanoseconds, resets the internal clock,
   * and schedules the first wake-up time.
   * @brief 该方法计算以毫秒和纳秒为单位的周期，重置内部时钟，并安排第一次唤醒时间。
   */
  void SetPeriod(double period);

  /**
   * @brief Retrieves the current loop period.
   * @brief 获取当前循环周期。
   *
   * @return Loop period in milliseconds as a double.
   * @return 以双精度浮点数表示的循环周期（毫秒）。
   */
  double GetPeriod() const;

  /**
   * @brief Sleeps until the next loop iteration based on the wake-up absolute time.
   * @brief 根据唤醒绝对时间休眠直到下一个循环迭代。
   *
   * This method puts the thread to sleep until the scheduled wake-up time and then schedules the next wake-up.
   * @brief 该方法使线程休眠直到预定的唤醒时间，然后安排下一个唤醒时间。
   */
  void SleepUntilNextLoop();

 private:
  /**
   * @brief Adds the loop period to the current wake-up time, handling nanosecond overflow.
   * @brief 将循环周期添加到当前唤醒时间，处理纳秒溢出。
   *
   * Updates the wake-up absolute time by adding the loop period, ensuring that nanoseconds do not exceed
   * one billion (the number of nanoseconds in a second).
   * @brief 通过添加循环周期更新唤醒绝对时间，确保纳秒不超过十亿（每秒的纳秒数）。
   */
  void AddPeriod();

  timespec wake_abs_time_; /**< Absolute wake-up time for the next loop iteration */
  /**< 下一个循环迭代的绝对唤醒时间 */
  long time_ns_; /**< Current nanosecond component of the wake-up time */
  /**< 唤醒时间的当前纳秒部分 */
  long time_s_; /**< Current second component of the wake-up time */
  /**< 唤醒时间的当前秒部分 */
  double period_ms_; /**< Loop period in milliseconds */
  /**< 循环周期，以毫秒为单位 */
  long period_ns_; /**< Loop period in nanoseconds */
  /**< 循环周期，以纳秒为单位 */

  // Constants for nanosecond calculations
  static constexpr long NS_CARRY = 999999999; /**< Nanosecond carry-over threshold */
  /**< 纳秒进位阈值 */
  static constexpr long NS_TO_S = 1000000000; /**< Number of nanoseconds in one second */
  /**< 一秒中的纳秒数 */
};

}  // namespace openrobot::ocm
