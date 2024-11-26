// task.hpp
#pragma once

#include <sys/timerfd.h>
#include "common/struct_type.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "node/node.hpp"
#include "task/task_base.hpp"

namespace openrobot::ocm {

/**
 * @class Task
 * @brief Represents a task within the OpenRobot OCM (Operational Control Module).
 *
 * Inherits from TaskBase and manages the initialization and execution of nodes associated with the task.
 * Each Task handles a collection of nodes, initializing them based on configuration settings and executing
 * their respective processes during runtime.
 */
class Task : public TaskBase {
 public:
  /**
   * @brief Constructor for Task.
   *
   * Initializes the TaskBase with task name, timer type, delay, and sets up node flags.
   *
   * @param task_setting Configuration settings for the task, including task name, timer settings, and node configurations.
   * @param node_list Shared pointer to a vector of node pointers associated with the task.
   * @param all_priority_enable Flag indicating whether all priority settings are enabled for the task.
   * @param all_cpu_affinity_enable Flag indicating whether all CPU affinity settings are enabled for the task.
   */
  Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
       bool all_cpu_affinity_enable);

  /**
   * @brief Default destructor.
   *
   * Cleans up any resources allocated by the Task.
   */
  ~Task() = default;

  /**
   * @brief Initializes all nodes associated with the task.
   *
   * Sets all nodes' initialization flags to true and calls InitNode to perform initialization.
   * This method ensures that all nodes are properly set up before the task begins execution.
   */
  void Init();

  /**
   * @brief Initializes a specific subset of nodes associated with the task.
   *
   * @param init_node_list Set of node names to initialize.
   * @return Set of node names that were successfully initialized.
   *
   * This method allows selective initialization of nodes based on the provided list.
   * Only nodes present in `init_node_list` will be initialized.
   */
  std::set<std::string> Init(const std::set<std::string>& init_node_list);

  /**
   * @brief Executes the task by running and optionally outputting each node.
   *
   * This method overrides the Run method from TaskBase and is called periodically based on the task's timer.
   * It iterates through all associated nodes, running them and handling their output if enabled.
   */
  void Run() override;

  /**
   * @brief Retrieves the task's configuration settings.
   *
   * @return Constant reference to the TaskSetting structure.
   *
   * Provides access to the task's configuration, including task name, timer settings, and node configurations.
   */
  const TaskSetting& GetTaskSetting() const;

 private:
  /**
   * @brief Initializes nodes based on their initialization flags.
   *
   * @param node_init_flag Map containing node names and their initialization status.
   *
   * This method iterates through the node list and initializes each node that has its initialization flag set to true.
   * It calls the Init and RunOnce methods on each node and handles their output if enabled.
   */
  void InitNode(const std::unordered_map<std::string, bool>& node_init_flag);

  /**
   * @brief Map to track whether each node should output data.
   *
   * The key is the node's name, and the value indicates whether output is enabled for that node.
   */
  std::unordered_map<std::string, bool> node_output_flag_;

  /**
   * @brief Configuration settings for the task.
   *
   * Contains all relevant settings such as task name, timer settings, and node configurations.
   */
  TaskSetting task_setting_;

  /**
   * @brief Map to track whether each node has been initialized.
   *
   * The key is the node's name, and the value indicates whether the node has been initialized.
   */
  std::unordered_map<std::string, bool> node_init_flag_;

  /**
   * @brief Shared pointer to the list of nodes associated with the task.
   *
   * Each node in the list represents a distinct component or process that the task manages and executes.
   */
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};

}  // namespace openrobot::ocm
