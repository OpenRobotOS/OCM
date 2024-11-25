// task.cpp
#include "task/task.hpp"

namespace openrobot::ocm {

// Constructor Implementation
Task::Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
           bool all_cpu_affinity_enable)
    : TaskBase(task_setting.task_name, task_setting.timer_setting.timer_type, static_cast<double>(task_setting.launch_setting.delay),
               all_priority_enable, all_cpu_affinity_enable),
      task_setting_(task_setting),
      node_list_(node_list) {
  // Set the task's execution period based on the configuration
  SetPeriod(task_setting_.timer_setting.period);

  // Initialize node output and initialization flags
  for (const auto& node : task_setting_.node_list) {
    node_output_flag_[node.node_name] = node.output_enable;
    node_init_flag_[node.node_name] = false;
  }
}

// Init Method Implementation
void Task::Init() {
  for (auto& node : node_init_flag_) {
    node.second = true;
  }
  InitNode(node_init_flag_);
}

// Overloaded Init Method Implementation
std::set<std::string> Task::Init(const std::set<std::string>& init_node_list) {
  std::set<std::string> init_node_list_result;
  for (auto& node : node_init_flag_) {
    if (init_node_list.find(node.first) != init_node_list.end()) {
      node.second = true;
      init_node_list_result.insert(node.first);
    }
  }
  InitNode(node_init_flag_);
  return init_node_list_result;
}

// Run Method Implementation
void Task::Run() {
  // Iterate through all nodes associated with the task
  for (auto& node : *node_list_) {
    const auto& node_name = node->GetNodeName();

    // If the node hasn't been initialized in this cycle, run it once and optionally output
    if (!node_init_flag_[node_name]) {
      node->RunOnce();
      if (node_output_flag_[node_name]) {
        node->Output();
      }
    } else {
      // Reset the initialization flag for the next cycle
      node_init_flag_.at(node_name) = false;
    }
  }
}

// GetTaskSetting Method Implementation
const TaskSetting& Task::GetTaskSetting() const { return task_setting_; }

// InitNode Method Implementation
void Task::InitNode(const std::unordered_map<std::string, bool>& node_init_flag) {
  for (auto& node : *node_list_) {
    if (node_init_flag.at(node->GetNodeName())) {
      node->Init();
      node->RunOnce();
      if (node_output_flag_[node->GetNodeName()]) {
        node->Output();
      }
    }
  }
}

}  // namespace openrobot::ocm
