#pragma once
#include <unordered_map>
#include <vector>
#include "node/node.hpp"
#include "node/node_map.hpp"
#include "task/task.hpp"

namespace openrobot::ocm {
struct NodeConfig {
  std::string name;
  bool enable;
};
struct TaskConfig {
  std::string name;
  std::vector<NodeConfig> node_list;
  double period;
  int priority;
  std::vector<int> cpu_affinity;
  std::vector<std::string> pre_node;
};
class Task : public TaskBase {
 public:
  Task(const std::string& thread_name, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list)
      : TaskBase(thread_name), node_list_(node_list) {}
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
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};
class Executer : public TaskBase {
 public:
  Executer(const std::string& thread_name, const std::shared_ptr<NodeMap>& node_map) : TaskBase(thread_name), node_map_(node_map) {}
  ~Executer() = default;

  void Init() {
    for (auto& task : resident_task_) {
      std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
      for (auto& node_config : task.node_list) {
        node_list->emplace_back(node_map_->GetNodePtr(node_config.name));
      }
      resident_task_list_.emplace_back(std::make_shared<Task>(task.name, node_list));
    }
  }
  void Run() override {}

 private:
  void Transition() {}
  std::vector<std::shared_ptr<Task>> resident_task_list_;
  std::unordered_map<std::string, std::shared_ptr<Task>> action_task_map_;
  std::vector<std::string> action_task_list_;
  std::shared_ptr<NodeMap> node_map_;
  std::vector<TaskConfig> resident_task_;
  std::vector<TaskConfig> action_task_;
};
}  // namespace openrobot::ocm
