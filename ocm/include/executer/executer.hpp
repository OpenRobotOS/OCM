#pragma once
#include <log_anywhere/log_anywhere.hpp>
#include <set>
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

  const TaskConfig& GetTaskConfig() const { return task_config_; }

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
    std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
    std::set<std::string> task_set_wait_to_start;
    for (auto& task : concurrent_group_task_list_) {
      for (auto& task_ptr : task.second) {
        task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr));
        task_set_wait_to_start.insert(task_ptr->GetTaskName());
      }
    }
    while (!task_set_wait_to_start.empty()) {
      bool is_pre_loop = true;
      for (auto& task : task_list_wait_to_start) {
        if (!task.first) {
          bool is_pre_node_empty = task.second->GetTaskConfig().launch_setting.pre_node.empty();
          const auto& pre_node_list = task.second->GetTaskConfig().launch_setting.pre_node;
          bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
            return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
          });
          if (is_pre_node_empty || is_pre_node_ready) {
            task.first = true;
            task.second->Init();
            task.second->TaskStart();
            task_set_wait_to_start.erase(task.second->GetTaskName());
            is_pre_loop = false;
            logger.info("Executer: Task {} start.", task.second->GetTaskName());
          }
        }
      }
      if (is_pre_loop) {
        throw std::runtime_error("The preceding node with a cycle exists.");
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

  openrobot::ocm::LogAnywhere& logger = openrobot::ocm::LogAnywhere::getInstance();
};
}  // namespace openrobot::ocm
