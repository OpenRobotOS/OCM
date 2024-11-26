// timer.cpp
#include "task/timer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>

namespace openrobot::ocm {

/**
 * @brief Constructs and starts the TimerOnce instance.
 *
 * Initializes the timer by capturing the current time.
 */
TimerOnce::TimerOnce() { start(); }

/**
 * @brief Starts or restarts the TimerOnce instance.
 *
 * Captures the current time as the start time.
 */
void TimerOnce::start() {
  if (clock_gettime(CLOCK_MONOTONIC, &_startTime) != 0) {
    std::cerr << "Failed to get start time: " << strerror(errno) << std::endl;
  }
}

/**
 * @brief Retrieves the elapsed time in milliseconds since the last start or getNs call.
 *
 * @return Elapsed time in milliseconds as a double.
 */
double TimerOnce::getMs() { return static_cast<double>(getNs()) / 1.e6; }

/**
 * @brief Retrieves the elapsed time in nanoseconds since the last start or getNs call.
 *
 * @return Elapsed time in nanoseconds as a 64-bit integer.
 */
int64_t TimerOnce::getNs() {
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    std::cerr << "Failed to get current time: " << strerror(errno) << std::endl;
    return 0;
  }

  int64_t ns = static_cast<int64_t>(now.tv_nsec - _startTime.tv_nsec) + static_cast<int64_t>(now.tv_sec - _startTime.tv_sec) * 1000000000LL;

  // Update the start time to the current time for the next measurement
  _startTime.tv_sec = now.tv_sec;
  _startTime.tv_nsec = now.tv_nsec;

  return ns;
}

/**
 * @brief Retrieves the elapsed time in seconds since the last start or getNs call.
 *
 * @return Elapsed time in seconds as a double.
 */
double TimerOnce::getSeconds() { return static_cast<double>(getNs()) / 1.e9; }

/**
 * @brief Retrieves the current time in milliseconds since the epoch.
 *
 * @return Current time in milliseconds as a double.
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
 *
 * Captures the current time as the wake-up absolute time and resets internal counters.
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
 *
 * @param period Period of the loop in seconds.
 *
 * This method calculates the period in milliseconds and nanoseconds, resets the internal clock,
 * and schedules the first wake-up time.
 */
void TimerLoop::SetPeriod(double period) {
  period_ms_ = period * 1000.0;
  period_ns_ = static_cast<long>(period * 1e9);
  ResetClock();
  AddPeriod();
}

/**
 * @brief Retrieves the current loop period.
 *
 * @return Loop period in milliseconds as a double.
 */
double TimerLoop::GetPeriod() const { return period_ms_; }

/**
 * @brief Sleeps until the next loop iteration based on the wake-up absolute time.
 *
 * This method puts the thread to sleep until the scheduled wake-up time and then schedules the next wake-up.
 */
void TimerLoop::SleepUntilNextLoop() {
  if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake_abs_time_, nullptr) != 0) {
    std::cerr << "Failed to sleep until next loop: " << strerror(errno) << std::endl;
  }
  AddPeriod();
}

/**
 * @brief Adds the loop period to the current wake-up time, handling nanosecond overflow.
 *
 * Updates the wake-up absolute time by adding the loop period, ensuring that nanoseconds do not exceed
 * one billion (the number of nanoseconds in a second).
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
