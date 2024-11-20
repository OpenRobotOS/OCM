#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace openrobot::ocm {
enum class NodeState : uint8_t { INIT = 0, RUNNING, STANDBY };
enum class TaskState : uint8_t { INIT = 0, RUNNING, STANDBY };
enum class TaskType : uint8_t {
  INTERNAL_TIMER = 0,
  EXTERNAL_TIMER,
  TRIGGER,
};

const std::unordered_map<std::string, TaskType> task_type_map = {
    {"INTERNAL_TIMER", TaskType::INTERNAL_TIMER},
    {"EXTERNAL_TIMER", TaskType::EXTERNAL_TIMER},
    {"TRIGGER", TaskType::TRIGGER},
};
}  // namespace openrobot::ocm
