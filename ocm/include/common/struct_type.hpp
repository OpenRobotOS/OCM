
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "common/enum.hpp"

namespace openrobot::ocm {

/**
 * @struct NodeConfig
 * @brief Configuration settings for a node.
 * @brief 节点的配置设置。
 *
 * This structure holds the configuration details for a single node,
 * including its name and output enablement status.
 * 该结构体包含单个节点的配置详情，包括其名称和输出启用状态。
 */
struct NodeConfig {
  std::string node_name; /**< The name identifier of the node. /**< 节点的名称标识符。 */
  bool output_enable;    /**< Flag indicating whether the node's output is enabled. /**< 标志，指示节点的输出是否被启用。 */
};

/**
 * @struct TimerSetting
 * @brief Configuration settings for a timer.
 * @brief 定时器的配置设置。
 *
 * This structure defines the settings for a timer, including its type
 * and the period duration.
 * 该结构体定义了定时器的设置，包括其类型和周期时长。
 */
struct TimerSetting {
  TimerType timer_type; /**< The type of the timer, as defined by `TimerType` enum. /**< 定时器的类型，由 `TimerType` 枚举定义。 */
  double period;        /**< The period of the timer in seconds. /**< 定时器的周期，单位为秒。 */
};

/**
 * @struct SystemSetting
 * @brief System configuration settings.
 * @brief 系统配置设置。
 *
 * This structure contains system-related settings such as priority and CPU affinity.
 * 该结构体包含与系统相关的设置，如优先级和CPU亲和性。
 */
struct SystemSetting {
  int priority;                  /**< The priority level of the system. /**< 系统的优先级级别。 */
  std::vector<int> cpu_affinity; /**< List of CPU cores the system is affined to. /**< 系统所关联的CPU核心列表。 */
};

/**
 * @struct LaunchSetting
 * @brief Configuration settings for task launch behavior.
 * @brief 任务启动行为的配置设置。
 *
 * This structure defines settings related to the launch behavior of tasks,
 * including pre-initialization nodes and launch delay.
 * 该结构体定义了与任务启动行为相关的设置，包括预初始化节点和启动延迟。
 */
struct LaunchSetting {
  std::vector<std::string> pre_node; /**< List of node names to be initialized before launching the task. /**< 启动任务前需初始化的节点名称列表。 */
  double delay;                      /**< Delay in seconds before launching the task. /**< 启动任务前的延迟时间（秒）。 */
};

/**
 * @struct TaskSetting
 * @brief Configuration settings for a single task.
 * @brief 单个任务的配置设置。
 *
 * This structure encapsulates all settings required to configure a task,
 * including associated nodes, timer settings, system settings, and launch behavior.
 * 该结构体封装了配置任务所需的所有设置，包括相关节点、定时器设置、系统设置和启动行为。
 */
struct TaskSetting {
  std::string task_name;             /**< The name identifier of the task. /**< 任务的名称标识符。 */
  std::vector<NodeConfig> node_list; /**< List of nodes associated with the task. /**< 与任务关联的节点列表。 */
  TimerSetting timer_setting;        /**< Timer settings for the task. /**< 任务的定时器设置。 */
  SystemSetting system_setting;      /**< System settings for the task. /**< 任务的系统设置。 */
  LaunchSetting launch_setting;      /**< Launch settings for the task. /**< 任务的启动设置。 */
};

/**
 * @struct GroupTaskSetting
 * @brief Configuration settings for a group task.
 * @brief 组任务的配置设置。
 *
 * This structure defines settings specific to group tasks, including force initialization nodes
 * and pre-initialization nodes.
 * 该结构体定义了特定于组任务的设置，包括强制初始化节点和预初始化节点。
 */
struct GroupTaskSetting {
  std::string task_name; /**< The name identifier of the group task. /**< 组任务的名称标识符。 */
  std::vector<std::string>
      force_init_node;               /**< List of node names to forcefully initialize for the group task. /**< 组任务需强制初始化的节点名称列表。 */
  std::vector<std::string> pre_node; /**< List of node names to be initialized before the group task. /**< 启动组任务前需初始化的节点名称列表。 */
};

/**
 * @struct GroupSetting
 * @brief Configuration settings for a task group.
 * @brief 任务组的配置设置。
 *
 * This structure encapsulates settings for a group of tasks, including the group name
 * and a list of tasks within the group.
 * 该结构体封装了一组任务的设置，包括组名称和组内的任务列表。
 */
struct GroupSetting {
  std::string group_name; /**< The name identifier of the group. /**< 组的名称标识符。 */
  std::unordered_map<std::string, GroupTaskSetting>
      task_list; /**< Mapping of task names to their respective group task settings. /**< 任务名称与其对应组任务设置的映射。 */
};

/**
 * @struct TaskList
 * @brief Lists of tasks categorized by group types.
 * @brief 按组类型分类的任务列表。
 *
 * This structure holds task settings organized into resident and standby groups.
 * 该结构体保存了组织成常驻组和待命组的任务设置。
 */
struct TaskList {
  std::unordered_map<std::string, TaskSetting>
      resident_group; /**< Mapping of task names to their settings for resident groups. /**< 常驻组任务名称与其设置的映射。 */
  std::unordered_map<std::string, TaskSetting>
      standby_group; /**< Mapping of task names to their settings for standby groups. /**< 待命组任务名称与其设置的映射。 */
};

/**
 * @struct ExecuterSetting
 * @brief Configuration settings for an executer.
 * @brief 执行器的配置设置。
 *
 * This structure defines the settings required to configure an executer, including package name,
 * timer settings, system settings, and flags for priority and CPU affinity.
 * 该结构体定义了配置执行器所需的设置，包括包名称、定时器设置、系统设置以及优先级和CPU亲和性的标志。
 */
struct ExecuterSetting {
  std::string package_name;          /**< The name of the package associated with the executer. /**< 与执行器关联的包名称。 */
  TimerSetting timer_setting;        /**< Timer settings for the executer. /**< 执行器的定时器设置。 */
  SystemSetting system_setting;      /**< System settings for the executer. /**< 执行器的系统设置。 */
  SystemSetting idle_system_setting; /**< System settings when the executer is idle. /**< 执行器空闲时的系统设置。 */
  bool all_priority_enable;          /**< Flag indicating if all priorities are enabled. /**< 标志，指示是否启用所有优先级。 */
  bool all_cpu_affinity_enable;      /**< Flag indicating if all CPU affinities are enabled. /**< 标志，指示是否启用所有CPU亲和性。 */
};

/**
 * @struct ExecuterConfig
 * @brief Comprehensive configuration for an executer.
 * @brief 执行器的全面配置。
 *
 * This structure encapsulates the complete configuration for an executer, including
 * executer settings, task lists, and exclusive task groups.
 * 该结构体封装了执行器的完整配置，包括执行器设置、任务列表和独占任务组。
 */
struct ExecuterConfig {
  ExecuterSetting executer_setting;                                   /**< Settings for the executer. /**< 执行器的设置。 */
  TaskList task_list;                                                 /**< Lists of tasks categorized by group types. /**< 按组类型分类的任务列表。 */
  std::unordered_map<std::string, GroupSetting> exclusive_task_group; /**< Mapping of group names to their respective group settings for exclusive
                                                                         tasks. /**< 组名称与其对应的独占任务组设置的映射。 */
};

}  // namespace openrobot::ocm
