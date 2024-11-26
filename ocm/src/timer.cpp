
#include "task/timer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>

namespace openrobot::ocm {

/**
 * @brief Constructs and starts the TimerOnce instance.
 * @brief 构造并启动 TimerOnce 实例。
 *
 * Initializes the timer by capturing the current time.
 * 通过捕获当前时间初始化计时器。
 */
TimerOnce::TimerOnce() { start(); }

/**
 * @brief Starts or restarts the TimerOnce instance.
 * @brief 启动或重启 TimerOnce 实例。
 *
 * Captures the current time as the start time.
 * 将当前时间捕获为开始时间。
 */
void TimerOnce::start() {
  if (clock_gettime(CLOCK_MONOTONIC, &_startTime) != 0) {
    std::cerr << "Failed to get start time: " << strerror(errno) << std::endl;
  }
}

/**
 * @brief Retrieves the elapsed time in milliseconds since the last start or getNs call.
 * @brief 获取自上次 start 或 getNs 调用以来的经过时间（毫秒）。
 *
 * @return Elapsed time in milliseconds as a double.
 * @return 经过时间（毫秒）。
 */
double TimerOnce::getMs() { return static_cast<double>(getNs()) / 1.e6; }

/**
 * @brief Retrieves the elapsed time in nanoseconds since the last start or getNs call.
 * @brief 获取自上次 start 或 getNs 调用以来的经过时间（纳秒）。
 *
 * @return Elapsed time in nanoseconds as a 64-bit integer.
 * @return 经过时间（64位整数，纳秒）。
 */
int64_t TimerOnce::getNs() {
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    std::cerr << "Failed to get current time: " << strerror(errno) << std::endl;
    return 0;
  }

  int64_t ns = static_cast<int64_t>(now.tv_nsec - _startTime.tv_nsec) + static_cast<int64_t>(now.tv_sec - _startTime.tv_sec) * 1000000000LL;

  // Update the start time to the current time for the next measurement
  // 更新下次测量的开始时间为当前时间
  _startTime.tv_sec = now.tv_sec;
  _startTime.tv_nsec = now.tv_nsec;

  return ns;
}

/**
 * @brief Retrieves the elapsed time in seconds since the last start or getNs call.
 * @brief 获取自上次 start 或 getNs 调用以来的经过时间（秒）。
 *
 * @return Elapsed time in seconds as a double.
 * @return 经过时间（秒）。
 */
double TimerOnce::getSeconds() { return static_cast<double>(getNs()) / 1.e9; }

/**
 * @brief Retrieves the current time in milliseconds since the epoch.
 * @brief 获取自纪元以来的当前时间（毫秒）。
 *
 * @return Current time in milliseconds as a double.
 * @return 当前时间（毫秒）。
 */
double TimerOnce::getNowTime() const {
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    std::cerr << "Failed to get current time: " << strerror(errno) << std::endl;
    return 0.0;
  }
  return static_cast<double>(now.tv_nsec + 1000000000LL * now.tv_sec) / 1.e6;
}

/**
 * @brief Resets the internal clock to the current time.
 * @brief 将内部时钟重置为当前时间。
 *
 * Captures the current time as the wake-up absolute time and resets internal counters.
 * 将当前时间捕获为唤醒绝对时间并重置内部计数器。
 */
void TimerLoop::ResetClock() {
  if (clock_gettime(CLOCK_MONOTONIC, &wake_abs_time_) != 0) {
    std::cerr << "Failed to reset clock: " << strerror(errno) << std::endl;
    return;
  }
  time_ns_ = wake_abs_time_.tv_nsec;
  time_s_ = wake_abs_time_.tv_sec;
}

/**
 * @brief Sets the loop period and initializes the wake-up time.
 * @brief 设置循环周期并初始化唤醒时间。
 *
 * @param period Period of the loop in seconds.
 * @param period 循环周期（秒）。
 *
 * This method calculates the period in milliseconds and nanoseconds, resets the internal clock,
 * and schedules the first wake-up time.
 * 该方法计算循环周期的毫秒和纳秒数，重置内部时钟，并安排首次唤醒时间。
 */
void TimerLoop::SetPeriod(double period) {
  period_ms_ = period * 1000.0;
  period_ns_ = static_cast<long>(period * 1e9);
  ResetClock();
  AddPeriod();
}

/**
 * @brief Retrieves the current loop period.
 * @brief 获取当前循环周期。
 *
 * @return Loop period in milliseconds as a double.
 * @return 循环周期（毫秒）。
 */
double TimerLoop::GetPeriod() const { return period_ms_; }

/**
 * @brief Sleeps until the next loop iteration based on the wake-up absolute time.
 * @brief 根据唤醒绝对时间休眠直到下一个循环迭代。
 *
 * This method puts the thread to sleep until the scheduled wake-up time and then schedules the next wake-up.
 * 该方法使线程休眠直到预定的唤醒时间，然后安排下一个唤醒。
 */
void TimerLoop::SleepUntilNextLoop() {
  if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake_abs_time_, nullptr) != 0) {
    std::cerr << "Failed to sleep until next loop: " << strerror(errno) << std::endl;
  }
  AddPeriod();
}

/**
 * @brief Adds the loop period to the current wake-up time, handling nanosecond overflow.
 * @brief 将循环周期添加到当前唤醒时间，处理纳秒溢出。
 *
 * Updates the wake-up absolute time by adding the loop period, ensuring that nanoseconds do not exceed
 * one billion (the number of nanoseconds in a second).
 * 通过添加循环周期更新唤醒绝对时间，确保纳秒不超过十亿（每秒的纳秒数）。
 */
void TimerLoop::AddPeriod() {
  time_ns_ += period_ns_;
  if (time_ns_ > NS_CARRY) {
    time_s_ += time_ns_ / NS_TO_S;
    time_ns_ %= NS_TO_S;
  }
  wake_abs_time_.tv_sec = time_s_;
  wake_abs_time_.tv_nsec = time_ns_;
}

}  // namespace openrobot::ocm
