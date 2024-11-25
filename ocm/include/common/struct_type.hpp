#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "common/enum.hpp"

namespace openrobot::ocm {

/**
 * @struct NodeConfig
 * @brief Configuration settings for a node.
 *
 * This structure holds the configuration details for a single node,
 * including its name and output enablement status.
 */
struct NodeConfig {
  std::string node_name; /**< The name identifier of the node. */
  bool output_enable;    /**< Flag indicating whether the node's output is enabled. */
};

/**
 * @struct TimerSetting
 * @brief Configuration settings for a timer.
 *
 * This structure defines the settings for a timer, including its type
 * and the period duration.
 */
struct TimerSetting {
  TimerType timer_type; /**< The type of the timer, as defined by `TimerType` enum. */
  double period;        /**< The period of the timer in seconds. */
};

/**
 * @struct SystemSetting
 * @brief System configuration settings.
 *
 * This structure contains system-related settings such as priority and CPU affinity.
 */
struct SystemSetting {
  int priority;                  /**< The priority level of the system. */
  std::vector<int> cpu_affinity; /**< List of CPU cores the system is affined to. */
};

/**
 * @struct LaunchSetting
 * @brief Configuration settings for task launch behavior.
 *
 * This structure defines settings related to the launch behavior of tasks,
 * including pre-initialization nodes and launch delay.
 */
struct LaunchSetting {
  std::vector<std::string> pre_node; /**< List of node names to be initialized before launching the task. */
  double delay;                      /**< Delay in seconds before launching the task. */
};

/**
 * @struct TaskSetting
 * @brief Configuration settings for a single task.
 *
 * This structure encapsulates all settings required to configure a task,
 * including associated nodes, timer settings, system settings, and launch behavior.
 */
struct TaskSetting {
  std::string task_name;             /**< The name identifier of the task. */
  std::vector<NodeConfig> node_list; /**< List of nodes associated with the task. */
  TimerSetting timer_setting;        /**< Timer settings for the task. */
  SystemSetting system_setting;      /**< System settings for the task. */
  LaunchSetting launch_setting;      /**< Launch settings for the task. */
};

/**
 * @struct GroupTaskSetting
 * @brief Configuration settings for a group task.
 *
 * This structure defines settings specific to group tasks, including force initialization nodes
 * and pre-initialization nodes.
 */
struct GroupTaskSetting {
  std::string task_name;                    /**< The name identifier of the group task. */
  std::vector<std::string> force_init_node; /**< List of node names to forcefully initialize for the group task. */
  std::vector<std::string> pre_node;        /**< List of node names to be initialized before the group task. */
};

/**
 * @struct GroupSetting
 * @brief Configuration settings for a task group.
 *
 * This structure encapsulates settings for a group of tasks, including the group name
 * and a list of tasks within the group.
 */
struct GroupSetting {
  std::string group_name;                                      /**< The name identifier of the group. */
  std::unordered_map<std::string, GroupTaskSetting> task_list; /**< Mapping of task names to their respective group task settings. */
};

/**
 * @struct TaskList
 * @brief Lists of tasks categorized by group types.
 *
 * This structure holds task settings organized into resident and standby groups.
 */
struct TaskList {
  std::unordered_map<std::string, TaskSetting> resident_group; /**< Mapping of task names to their settings for resident groups. */
  std::unordered_map<std::string, TaskSetting> standby_group;  /**< Mapping of task names to their settings for standby groups. */
};

/**
 * @struct ExecuterSetting
 * @brief Configuration settings for an executer.
 *
 * This structure defines the settings required to configure an executer, including package name,
 * timer settings, system settings, and flags for priority and CPU affinity.
 */
struct ExecuterSetting {
  std::string package_name;          /**< The name of the package associated with the executer. */
  TimerSetting timer_setting;        /**< Timer settings for the executer. */
  SystemSetting system_setting;      /**< System settings for the executer. */
  SystemSetting idle_system_setting; /**< System settings when the executer is idle. */
  bool all_priority_enable;          /**< Flag indicating if all priorities are enabled. */
  bool all_cpu_affinity_enable;      /**< Flag indicating if all CPU affinities are enabled. */
};

/**
 * @struct ExecuterConfig
 * @brief Comprehensive configuration for an executer.
 *
 * This structure encapsulates the complete configuration for an executer, including
 * executer settings, task lists, and exclusive task groups.
 */
struct ExecuterConfig {
  ExecuterSetting executer_setting; /**< Settings for the executer. */
  TaskList task_list;               /**< Lists of tasks categorized by group types. */
  std::unordered_map<std::string, GroupSetting>
      exclusive_task_group; /**< Mapping of group names to their respective group settings for exclusive tasks. */
};

}  // namespace openrobot::ocm
