#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace openrobot::ocm {

/**
 * @enum NodeState
 * @brief Represents the state of a node.
 */
enum class NodeState : uint8_t {
  INIT = 0, /**< Initialization state */
  RUNNING,  /**< Running state */
  STANDBY   /**< Standby state */
};

/**
 * @enum TaskState
 * @brief Represents the state of a task.
 */
enum class TaskState : uint8_t {
  INIT = 0, /**< Initialization state */
  RUNNING,  /**< Running state */
  STANDBY   /**< Standby state */
};

/**
 * @enum TimerType
 * @brief Represents the type of a timer.
 */
enum class TimerType : uint8_t {
  INTERNAL_TIMER = 0, /**< Internal timer */
  EXTERNAL_TIMER,     /**< External timer */
  TRIGGER             /**< Trigger */
};

/**
 * @brief Maps string representations of timer types to their corresponding `TimerType` enum values.
 *
 * This map is used to convert string-based timer type identifiers to their respective `TimerType` enumeration.
 */
const std::unordered_map<std::string, TimerType> timer_type_map = {
    {"INTERNAL_TIMER", TimerType::INTERNAL_TIMER},
    {"EXTERNAL_TIMER", TimerType::EXTERNAL_TIMER},
    {"TRIGGER", TimerType::TRIGGER},
};

/**
 * @namespace openrobot::ocm
 * @brief Namespace for OpenRobot's Operation Control Module (OCM).
 */
}  // namespace openrobot::ocm
