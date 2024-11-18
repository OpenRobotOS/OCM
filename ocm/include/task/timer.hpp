#pragma once

#include <stdint.h>
#include <time.h>

/*!
 * TimerOnce for measuring time elapsed with clock_monotonic
 */
namespace openrobot::ocm {
#define NS_CARRY 999999999
#define NS_TO_S 1000000000

class TimerOnce {
 public:
  /*!
   * Construct and start timer
   */
  explicit TimerOnce() { start(); }

  /*!
   * Start the timer
   */
  void start() { clock_gettime(CLOCK_MONOTONIC, &_startTime); }
  /*!
   * Get milliseconds elapsed
   */
  double getMs() { return static_cast<double>(getNs()) / 1.e6; }

  /*!
   * Get nanoseconds elapsed
   */
  int64_t getNs() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t ns = (int64_t)(now.tv_nsec - _startTime.tv_nsec) + 1000000000 * (now.tv_sec - _startTime.tv_sec);
    _startTime.tv_sec = now.tv_sec;
    _startTime.tv_nsec = now.tv_nsec;
    return ns;
  }

  /*!
   * Get seconds elapsed
   */
  double getSeconds() { return static_cast<double>(getNs()) / 1.e9; }

  /*!
   * Get now time in milliseconds
   */
  double getNowTime() const {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<double>(now.tv_nsec + 1000000000 * now.tv_sec) / 1.e6;
  }

 private:
  struct timespec _startTime;
};

class TimerLoop {
 public:
  void ResetClock() {
    clock_gettime(CLOCK_MONOTONIC, &wake_abs_time_);
    time_ns_ = wake_abs_time_.tv_nsec;
    time_s_ = wake_abs_time_.tv_sec;
  }
  void SetPeriod(double period) {
    period_ms_ = period * 1000;
    period_ns_ = static_cast<long>(period * 1000 * 1000 * 1000);
    ResetClock();
    AddPeriod();
  }

  double GetPeriod() const { return period_ms_; }

  void SleepUntilNextLoop() {
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake_abs_time_, nullptr);
    AddPeriod();
  }

 private:
  void AddPeriod() {
    time_ns_ = time_ns_ + period_ns_;
    if (time_ns_ > NS_CARRY) {
      time_s_ += time_ns_ / NS_TO_S;
      time_ns_ = time_ns_ % NS_TO_S;
    }
    wake_abs_time_.tv_sec = time_s_;
    wake_abs_time_.tv_nsec = time_ns_;
  }

  timespec wake_abs_time_;
  long time_ns_;
  long time_s_;
  double period_ms_;
  long period_ns_;
};
}  // namespace openrobot::ocm
