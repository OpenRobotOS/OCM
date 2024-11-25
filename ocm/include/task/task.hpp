// task.hpp
#pragma once

#include <log_anywhere/log_anywhere.hpp>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "common/struct_type.hpp"
#include "node/node.hpp"
#include "task/task_base.hpp"

namespace openrobot::ocm {

/**
 * @class Task
 * @brief Represents a task within the OpenRobot OCM (Operational Control Module).
 *
 * Inherits from TaskBase and manages the initialization and execution of nodes associated with the task.
 */
class Task : public TaskBase {
 public:
  /**
   * @brief Constructor for Task.
   *
   * Initializes the TaskBase with task name, timer type, delay, and sets up node flags.
   *
   * @param task_setting Configuration settings for the task.
   * @param node_list Shared pointer to a vector of node pointers associated with the task.
   */
  Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
       bool all_cpu_affinity_enable);

  /**
   * @brief Default destructor.
   */
  ~Task() = default;

  /**
   * @brief Initializes all nodes associated with the task.
   *
   * Sets all nodes' initialization flags to true and calls InitNode to perform initialization.
   */
  void Init();

  /**
   * @brief Initializes a specific subset of nodes associated with the task.
   *
   * @param init_node_list Set of node names to initialize.
   * @return Set of node names that were successfully initialized.
   */
  std::set<std::string> Init(const std::set<std::string>& init_node_list);

  /**
   * @brief Executes the task by running and optionally outputting each node.
   *
   * This method overrides the Run method from TaskBase and is called periodically based on the task's timer.
   */
  void Run() override;

  /**
   * @brief Retrieves the task's configuration settings.
   *
   * @return Constant reference to the TaskSetting structure.
   */
  const TaskSetting& GetTaskSetting() const;

 private:
  /**
   * @brief Initializes nodes based on their initialization flags.
   *
   * @param node_init_flag Map containing node names and their initialization status.
   */
  void InitNode(const std::unordered_map<std::string, bool>& node_init_flag);

  // Map to track whether each node should output data
  std::unordered_map<std::string, bool> node_output_flag_;

  // Configuration settings for the task
  TaskSetting task_setting_;

  // Map to track whether each node has been initialized
  std::unordered_map<std::string, bool> node_init_flag_;

  // Shared pointer to the list of nodes associated with the task
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};

}  // namespace openrobot::ocm
