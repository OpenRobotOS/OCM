#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "common/enum.hpp"

namespace openrobot::ocm {
struct NodeConfig {
  std::string node_name;
  bool output_enable;
};
struct TaskSetting {
  TaskType timer_type;
  double period;
};
struct SystemSetting {
  double priority;
  std::vector<double> cpu_affinity;
};
struct LaunchSetting {
  std::vector<std::string> pre_node;
  double delay;
};
struct TaskConfig {
  std::string task_name;
  std::vector<NodeConfig> node_list;
  TaskSetting task_setting;
  SystemSetting system_setting;
  LaunchSetting launch_setting;
};
struct GroupConfig {
  std::string group_name;
  std::vector<TaskConfig> task_list;
};
struct ExecuterConfig {
  std::string package_name;
  std::string sem_name;
  double period;
  std::unordered_map<std::string, GroupConfig> concurrent_group;
  std::unordered_map<std::string, GroupConfig> exclusive_group;
};
}  // namespace openrobot::ocm
