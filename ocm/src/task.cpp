// task.cpp
#include "task/task.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a `Task` instance.
 *
 * Initializes the TaskBase with the provided settings and sets up node flags based on the task configuration.
 * It also initializes the execution period and logs the creation of the task thread.
 *
 * @param task_setting Configuration settings for the task, including task name, timer settings, and node configurations.
 * @param node_list Shared pointer to a vector of node pointers associated with the task.
 * @param all_priority_enable Flag indicating whether all priority settings are enabled for the task.
 * @param all_cpu_affinity_enable Flag indicating whether all CPU affinity settings are enabled for the task.
 */
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

/**
 * @brief Initializes all nodes associated with the task.
 *
 * Sets all nodes' initialization flags to true and calls InitNode to perform initialization.
 * This ensures that all nodes are properly initialized before the task starts running.
 */
void Task::Init() {
  for (auto& node : node_init_flag_) {
    node.second = true;
  }
  InitNode(node_init_flag_);
}

/**
 * @brief Initializes a specific subset of nodes associated with the task.
 *
 * @param init_node_list Set of node names to initialize.
 * @return Set of node names that were successfully initialized.
 *
 * This method allows selective initialization of nodes based on the provided list.
 * Only nodes present in `init_node_list` will have their initialization flags set to true and be initialized.
 */
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

/**
 * @brief Executes the task by running and optionally outputting each node.
 *
 * Iterates through all nodes associated with the task. For each node, it checks if the node has been
 * initialized in the current cycle. If not, it runs the node once and outputs its data if enabled.
 * After execution, it resets the node's initialization flag for the next cycle.
 */
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

/**
 * @brief Retrieves the task's configuration settings.
 *
 * @return Constant reference to the TaskSetting structure.
 *
 * Provides access to the task's configuration, including task name, timer settings, and node configurations.
 */
const TaskSetting& Task::GetTaskSetting() const { return task_setting_; }

/**
 * @brief Initializes nodes based on their initialization flags.
 *
 * @param node_init_flag Map containing node names and their initialization status.
 *
 * This method iterates through the node list and initializes each node that has its initialization flag set to true.
 * It calls the Init and RunOnce methods on each node and handles their output if enabled.
 */
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
