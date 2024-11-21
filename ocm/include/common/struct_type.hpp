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
struct TimerSetting {
  TimerType timer_type;
  double period;
};
struct SystemSetting {
  double priority;
  std::vector<double> cpu_affinity;
};
struct ExecuterSystemSetting {
  double priority;
  std::vector<double> executer_cpu_affinity;
  std::vector<double> idle_task_cpu_affinity;
};
struct LaunchSetting {
  std::vector<std::string> pre_node;
  double delay;
};
struct TaskSetting {
  std::string task_name;
  std::vector<NodeConfig> node_list;
  TimerSetting timer_setting;
  SystemSetting system_setting;
  LaunchSetting launch_setting;
};

struct GroupTaskSetting {
  std::string task_name;
  std::vector<std::string> force_init_node;
  std::vector<std::string> pre_node;
};

struct GroupSetting {
  std::string group_name;
  std::unordered_map<std::string, GroupTaskSetting> task_list;
};

struct TaskList {
  std::unordered_map<std::string, TaskSetting> resident_group;
  std::unordered_map<std::string, TaskSetting> standby_group;
};

struct ExecuterSetting {
  std::string package_name;
  std::string sem_name;
  TimerSetting timer_setting;
  ExecuterSystemSetting system_setting;
};

struct ExecuterConfig {
  ExecuterSetting executer_setting;
  TaskList task_list;
  std::unordered_map<std::string, GroupSetting> exclusive_task_group;
};
}  // namespace openrobot::ocm
