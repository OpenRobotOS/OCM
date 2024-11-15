#pragma once

#include <stdint.h>
#include <time.h>

/*!
 * Timer for measuring time elapsed with clock_monotonic
 */
namespace raise_ctrl {
class Timer {
 public:
  /*!
   * Construct and start timer
   */
  explicit Timer() { start(); }

  /*!
   * Start the timer
   */
  void start() {
    clock_gettime(CLOCK_MONOTONIC, &_startTime);
    _nowTime = ((_startTime.tv_nsec) + 1000000000 * (_startTime.tv_sec)) / 1.e6;
  }
  /*!
   * Get milliseconds elapsed
   */
  double getMs() const { return static_cast<double>(getNs()) / 1.e6; }

  /*!
   * Get nanoseconds elapsed
   */
  int64_t getNs() const {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (int64_t)(now.tv_nsec - _startTime.tv_nsec) + 1000000000 * (now.tv_sec - _startTime.tv_sec);
  }

  /*!
   * Get seconds elapsed
   */
  double getSeconds() const { return static_cast<double>(getNs()) / 1.e9; }

  struct timespec _startTime;
  double _nowTime;
};
}  // namespace raise_ctrl