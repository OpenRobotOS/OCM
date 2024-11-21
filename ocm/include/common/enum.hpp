#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace openrobot::ocm {
enum class NodeState : uint8_t { INIT = 0, RUNNING, STANDBY };
enum class TaskState : uint8_t { INIT = 0, RUNNING, STANDBY };
enum class TimerType : uint8_t {
  INTERNAL_TIMER = 0,
  EXTERNAL_TIMER,
  TRIGGER,
};

const std::unordered_map<std::string, TimerType> timer_type_map = {
    {"INTERNAL_TIMER", TimerType::INTERNAL_TIMER},
    {"EXTERNAL_TIMER", TimerType::EXTERNAL_TIMER},
    {"TRIGGER", TimerType::TRIGGER},
};
}  // namespace openrobot::ocm
