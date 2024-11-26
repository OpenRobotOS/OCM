// Start of Selection
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace openrobot::ocm {

/**
 * @enum NodeState
 * @brief 表示节点的状态。Represents the state of a node.
 */
enum class NodeState : uint8_t {
  INIT = 0, /**< 初始化状态 Initialization state */
  RUNNING,  /**< 运行状态 Running state */
  STANDBY   /**< 待命状态 Standby state */
};

/**
 * @enum TaskState
 * @brief 表示任务的状态。Represents the state of a task.
 */
enum class TaskState : uint8_t {
  INIT = 0, /**< 初始化状态 Initialization state */
  RUNNING,  /**< 运行状态 Running state */
  STANDBY   /**< 待命状态 Standby state */
};

/**
 * @enum TimerType
 * @brief 表示定时器的类型。Represents the type of a timer.
 */
enum class TimerType : uint8_t {
  INTERNAL_TIMER = 0, /**< 内部定时器 Internal timer */
  EXTERNAL_TIMER,     /**< 外部定时器 External timer */
  TRIGGER             /**< 触发器 Trigger */
};

/**
 * @brief 将定时器类型的字符串表示映射到对应的 `TimerType` 枚举值。
 * Maps string representations of timer types to their corresponding `TimerType` enum values.
 *
 * 此映射用于将基于字符串的定时器类型标识符转换为各自的 `TimerType` 枚举。
 * This map is used to convert string-based timer type identifiers to their respective `TimerType` enumeration.
 */
const std::unordered_map<std::string, TimerType> timer_type_map = {
    {"INTERNAL_TIMER", TimerType::INTERNAL_TIMER},
    {"EXTERNAL_TIMER", TimerType::EXTERNAL_TIMER},
    {"TRIGGER", TimerType::TRIGGER},
};

/**
 * @namespace openrobot::ocm
 * @brief OpenRobot操作控制模块 (OCM) 的命名空间。
 * Namespace for OpenRobot's Operation Control Module (OCM).
 */
}  // namespace openrobot::ocm
// End of Selection