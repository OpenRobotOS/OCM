#pragma once
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

namespace openrobot::ocm {
namespace auto_Task {
namespace auto_TaskSetting {
class NodeList {
 public:
  NodeList() = default;
  ~NodeList() = default;

  NodeList(const NodeList&) = default;
  NodeList& operator=(const NodeList&) = default;

  NodeList(NodeList&&) = default;
  NodeList& operator=(NodeList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
    if (auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
  }

  std::string NodeName() const { return node_name_; }

  bool OutputEnable() const { return output_enable_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "NodeList:" << std::endl;
    std::cout << indent << "    node_name_: " << node_name_ << std::endl;
    std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
  }

 private:
  std::string node_name_;
  bool output_enable_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class TaskSetting {
 public:
  TaskSetting() = default;
  ~TaskSetting() = default;

  TaskSetting(const TaskSetting&) = default;
  TaskSetting& operator=(const TaskSetting&) = default;

  TaskSetting(TaskSetting&&) = default;
  TaskSetting& operator=(TaskSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
    if (auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
  }

  std::string TimerType() const { return timer_type_; }

  double Period() const { return period_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TaskSetting:" << std::endl;
    std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
    std::cout << indent << "    period_: " << period_ << std::endl;
  }

 private:
  std::string timer_type_;
  double period_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class SystemSetting {
 public:
  SystemSetting() = default;
  ~SystemSetting() = default;

  SystemSetting(const SystemSetting&) = default;
  SystemSetting& operator=(const SystemSetting&) = default;

  SystemSetting(SystemSetting&&) = default;
  SystemSetting& operator=(SystemSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
    if (auto_yaml_node["cpu_affinity"]) {
      cpu_affinity_.clear();
      for (auto& item : auto_yaml_node["cpu_affinity"]) {
        cpu_affinity_.push_back(item.as<std::string>());
      }
    }
  }

  double Priority() const { return priority_; }

  std::vector<std::string> CpuAffinity() const { return cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "SystemSetting:" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: [" << std::endl;
    for (const auto& item : cpu_affinity_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  double priority_;
  std::vector<std::string> cpu_affinity_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class LaunchSetting {
 public:
  LaunchSetting() = default;
  ~LaunchSetting() = default;

  LaunchSetting(const LaunchSetting&) = default;
  LaunchSetting& operator=(const LaunchSetting&) = default;

  LaunchSetting(LaunchSetting&&) = default;
  LaunchSetting& operator=(LaunchSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["pre_node"]) {
      pre_node_.clear();
      for (auto& item : auto_yaml_node["pre_node"]) {
        pre_node_.push_back(item.as<std::string>());
      }
    }
    if (auto_yaml_node["delay"]) delay_ = auto_yaml_node["delay"].as<double>();
  }

  std::vector<std::string> PreNode() const { return pre_node_; }

  double Delay() const { return delay_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "LaunchSetting:" << std::endl;
    std::cout << indent << "    pre_node_: [" << std::endl;
    for (const auto& item : pre_node_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    delay_: " << delay_ << std::endl;
  }

 private:
  std::vector<std::string> pre_node_;
  double delay_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class TaskList {
 public:
  TaskList() = default;
  ~TaskList() = default;

  TaskList(const TaskList&) = default;
  TaskList& operator=(const TaskList&) = default;

  TaskList(TaskList&&) = default;
  TaskList& operator=(TaskList&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
    if (auto_yaml_node["node_list"]) {
      node_list_.clear();
      for (auto& item : auto_yaml_node["node_list"]) {
        auto_Task::auto_TaskSetting::NodeList elem;
        elem.update_from_yaml(item);
        node_list_.push_back(elem);
      }
    }
    if (auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
    if (auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    if (auto_yaml_node["launch_setting"]) launch_setting_.update_from_yaml(auto_yaml_node["launch_setting"]);
  }

  std::string TaskName() const { return task_name_; }

  std::vector<auto_Task::auto_TaskSetting::NodeList> NodeList() const { return node_list_; }

  const auto_Task::auto_TaskSetting::TaskSetting& TaskSetting() const { return task_setting_; }

  const auto_Task::auto_TaskSetting::SystemSetting& SystemSetting() const { return system_setting_; }

  const auto_Task::auto_TaskSetting::LaunchSetting& LaunchSetting() const { return launch_setting_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "TaskList:" << std::endl;
    std::cout << indent << "    task_name_: " << task_name_ << std::endl;
    std::cout << indent << "    node_list_: [" << std::endl;
    for (const auto& item : node_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    task_setting_:" << std::endl;
    task_setting_.print(indent_level + 1);
    std::cout << indent << "    system_setting_:" << std::endl;
    system_setting_.print(indent_level + 1);
    std::cout << indent << "    launch_setting_:" << std::endl;
    launch_setting_.print(indent_level + 1);
  }

 private:
  std::string task_name_;
  std::vector<auto_Task::auto_TaskSetting::NodeList> node_list_;
  auto_Task::auto_TaskSetting::TaskSetting task_setting_;
  auto_Task::auto_TaskSetting::SystemSetting system_setting_;
  auto_Task::auto_TaskSetting::LaunchSetting launch_setting_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class ConcurrentGroup {
 public:
  ConcurrentGroup() = default;
  ~ConcurrentGroup() = default;

  ConcurrentGroup(const ConcurrentGroup&) = default;
  ConcurrentGroup& operator=(const ConcurrentGroup&) = default;

  ConcurrentGroup(ConcurrentGroup&&) = default;
  ConcurrentGroup& operator=(ConcurrentGroup&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["group_name"]) group_name_ = auto_yaml_node["group_name"].as<std::string>();
    if (auto_yaml_node["task_list"]) {
      task_list_.clear();
      for (auto& item : auto_yaml_node["task_list"]) {
        auto_Task::auto_TaskSetting::TaskList elem;
        elem.update_from_yaml(item);
        task_list_.push_back(elem);
      }
    }
  }

  std::string GroupName() const { return group_name_; }

  std::vector<auto_Task::auto_TaskSetting::TaskList> TaskList() const { return task_list_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ConcurrentGroup:" << std::endl;
    std::cout << indent << "    group_name_: " << group_name_ << std::endl;
    std::cout << indent << "    task_list_: [" << std::endl;
    for (const auto& item : task_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::string group_name_;
  std::vector<auto_Task::auto_TaskSetting::TaskList> task_list_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class ExclusiveGroup {
 public:
  ExclusiveGroup() = default;
  ~ExclusiveGroup() = default;

  ExclusiveGroup(const ExclusiveGroup&) = default;
  ExclusiveGroup& operator=(const ExclusiveGroup&) = default;

  ExclusiveGroup(ExclusiveGroup&&) = default;
  ExclusiveGroup& operator=(ExclusiveGroup&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["group_name"]) group_name_ = auto_yaml_node["group_name"].as<std::string>();
    if (auto_yaml_node["task_list"]) {
      task_list_.clear();
      for (auto& item : auto_yaml_node["task_list"]) {
        auto_Task::auto_TaskSetting::TaskList elem;
        elem.update_from_yaml(item);
        task_list_.push_back(elem);
      }
    }
  }

  std::string GroupName() const { return group_name_; }

  std::vector<auto_Task::auto_TaskSetting::TaskList> TaskList() const { return task_list_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ExclusiveGroup:" << std::endl;
    std::cout << indent << "    group_name_: " << group_name_ << std::endl;
    std::cout << indent << "    task_list_: [" << std::endl;
    for (const auto& item : task_list_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::string group_name_;
  std::vector<auto_Task::auto_TaskSetting::TaskList> task_list_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

namespace auto_Task {
namespace auto_TaskSetting {
class auto_TaskSetting {
 public:
  auto_TaskSetting() = default;
  ~auto_TaskSetting() = default;

  auto_TaskSetting(const auto_TaskSetting&) = default;
  auto_TaskSetting& operator=(const auto_TaskSetting&) = default;

  auto_TaskSetting(auto_TaskSetting&&) = default;
  auto_TaskSetting& operator=(auto_TaskSetting&&) = default;

  void update_from_yaml(const YAML::Node& auto_yaml_node) {
    if (auto_yaml_node["concurrent_group"]) {
      concurrent_group_.clear();
      for (auto& item : auto_yaml_node["concurrent_group"]) {
        auto_Task::auto_TaskSetting::ConcurrentGroup elem;
        elem.update_from_yaml(item);
        concurrent_group_.push_back(elem);
      }
    }
    if (auto_yaml_node["exclusive_group"]) {
      exclusive_group_.clear();
      for (auto& item : auto_yaml_node["exclusive_group"]) {
        auto_Task::auto_TaskSetting::ExclusiveGroup elem;
        elem.update_from_yaml(item);
        exclusive_group_.push_back(elem);
      }
    }
  }

  std::vector<auto_Task::auto_TaskSetting::ConcurrentGroup> ConcurrentGroup() const { return concurrent_group_; }

  std::vector<auto_Task::auto_TaskSetting::ExclusiveGroup> ExclusiveGroup() const { return exclusive_group_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "auto_TaskSetting:" << std::endl;
    std::cout << indent << "    concurrent_group_: [" << std::endl;
    for (const auto& item : concurrent_group_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    exclusive_group_: [" << std::endl;
    for (const auto& item : exclusive_group_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::vector<auto_Task::auto_TaskSetting::ConcurrentGroup> concurrent_group_;
  std::vector<auto_Task::auto_TaskSetting::ExclusiveGroup> exclusive_group_;
};

}  // namespace auto_TaskSetting
}  // namespace auto_Task

class ConfigCollect {
 public:
  ConfigCollect() = default;
  ~ConfigCollect() = default;

  ConfigCollect(const ConfigCollect&) = delete;
  ConfigCollect& operator=(const ConfigCollect&) = delete;

  ConfigCollect(ConfigCollect&&) = delete;
  ConfigCollect& operator=(ConfigCollect&&) = delete;

  void update_from_yaml(const std::string& name, const std::string& base_path) {
    if (name.empty()) {
      return;
    }
    bool matched = false;
    if (name == "task_task_setting") {
      update_from_yaml_task_task_setting(base_path);
      matched = true;
    }
    if (!matched) {
      // 未找到匹配的配置项
      std::cerr << "[ConfigCollect] No matching configuration for '" << name << "'" << std::endl;
    }
  }

  void update_from_yaml_all(const std::string& base_path) { update_from_yaml_task_task_setting(base_path); }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "Task_TaskSetting:" << std::endl;
    {
      std::shared_lock<std::shared_mutex> lock(m_Task_TaskSetting);
      Task_TaskSetting.print(indent_level + 1);
    }
  }

  auto_Task::auto_TaskSetting::auto_TaskSetting get_Task_TaskSetting() const {
    std::shared_lock<std::shared_mutex> lock(m_Task_TaskSetting);
    return Task_TaskSetting;
  }

 private:
  auto_Task::auto_TaskSetting::auto_TaskSetting Task_TaskSetting;
  mutable std::shared_mutex m_Task_TaskSetting;

  void update_from_yaml_task_task_setting(const std::string& base_path) {
    YAML::Node auto_yaml_node;
    // 更新 auto_Task::auto_TaskSetting::auto_TaskSetting
    auto_yaml_node = YAML::LoadFile(base_path + "/task/task_setting.yaml");
    {
      std::unique_lock<std::shared_mutex> lock(m_Task_TaskSetting);
      Task_TaskSetting.update_from_yaml(auto_yaml_node);
    }
  }
};

}  // namespace openrobot::ocm
