#pragma once
#include <unordered_map>
#include <vector>
#include "node/node.hpp"
#include "node/node_map.hpp"
#include "ocm/ocm.hpp"
#include "task/task.hpp"

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
  int priority;
  std::vector<int> cpu_affinity;
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

class Task : public TaskBase {
 public:
  Task(const TaskConfig& task_config, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list)
      : TaskBase(task_config.task_name, task_config.task_setting.timer_type, static_cast<double>(task_config.launch_setting.delay),
                 task_config.task_name),
        task_config_(task_config),
        node_list_(node_list) {
    SetPeriod(task_config_.task_setting.period);
  }
  ~Task() = default;
  void Init() {
    for (auto& node : *node_list_) {
      node->Init();
      node->Run();
    }
  }
  void Run() override {
    for (auto& node : *node_list_) {
      node->Run();
    }
  }

 private:
  TaskConfig task_config_;
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};
class Executer : public TaskBase {
 public:
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map)
      : TaskBase(executer_config.package_name, TaskType::INTERNAL_TIMER, 0.0, executer_config.sem_name),
        node_map_(node_map),
        executer_config_(executer_config) {
    SetPeriod(executer_config_.period);
    TaskStart();
    CreateTask();
  }
  ~Executer() = default;

  void Init() {
    for (auto& task : concurrent_group_task_list_) {
      for (auto& task_ptr : task.second) {
        task_ptr->TaskStart();
      }
    }
  }

 private:
  void Run() override { std::cout << "Executer Run" << std::endl; }
  void CreateTask() {
    for (auto& group_config : executer_config_.concurrent_group) {
      for (auto& task_config : group_config.second.task_list) {
        std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
        for (auto& node_config : task_config.node_list) {
          node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
        }
        concurrent_group_task_list_[task_config.task_name].emplace_back(std::make_shared<Task>(task_config, node_list));
      }
    }
    for (auto& group_config : executer_config_.exclusive_group) {
      for (auto& task_config : group_config.second.task_list) {
        std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
        for (auto& node_config : task_config.node_list) {
          node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
        }
        exclusive_group_task_list_[task_config.task_name].emplace_back(std::make_shared<Task>(task_config, node_list));
      }
    }
  }
  void Transition() {}
  std::unordered_map<std::string, std::vector<std::shared_ptr<Task>>> concurrent_group_task_list_;
  std::unordered_map<std::string, std::vector<std::shared_ptr<Task>>> exclusive_group_task_list_;
  std::shared_ptr<NodeMap> node_map_;
  ExecuterConfig executer_config_;
  AtomicPtr<std::string> target_state_;
};
}  // namespace openrobot::ocm
