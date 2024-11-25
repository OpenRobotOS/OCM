// executer.hpp
#pragma once

#include <log_anywhere/log_anywhere.hpp>
#include <memory>
#include <set>
#include <string>
#include "common/struct_type.hpp"
#include "node/node_map.hpp"
#include "ocm/ocm.hpp"
#include "task/task.hpp"

namespace openrobot::ocm {

/**
 * @class Executer
 * @brief Manages the execution and transition of task groups within the OpenRobot Operation Control Module (OCM).
 *
 * The Executer class inherits from TaskBase and is responsible for creating, initializing,
 * and transitioning between different task groups based on the provided configuration.
 * It ensures that tasks are started and stopped appropriately, handling dependencies and
 * exclusive task groups.
 */
class Executer : public TaskBase {
 public:
  /**
   * @brief Constructor for Executer.
   *
   * Initializes the Executer with its configuration, sets up logging, and initializes internal state variables.
   *
   * @param executer_config Configuration settings for the Executer.
   * @param node_map Shared pointer to the NodeMap for retrieving node pointers.
   */
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map, const std::string& desired_group_topic_name);

  /**
   * @brief Default destructor.
   *
   * Cleans up resources used by the Executer.
   */
  ~Executer() = default;

  /**
   * @brief Creates tasks for both resident and standby groups based on the configuration.
   *
   * Retrieves node pointers from the NodeMap and initializes Task instances for each task in the configuration.
   * Also populates the set of exclusive task groups.
   */
  void CreateTask();

  /**
   * @brief Initializes all tasks in the resident group, ensuring that pre-requisite nodes are ready.
   *
   * Waits until all tasks are ready to start based on their pre-node dependencies.
   */
  void InitTask();

  /**
   * @brief Executes the main run loop for the Executer.
   *
   * Checks for transitions between task groups and handles them accordingly.
   */
  void Run() override;

  /**
   * @brief Exits all tasks gracefully.
   *
   * Stops and destroys all tasks in both resident and standby groups, ensuring a clean shutdown.
   */
  void ExitAllTask();

 private:
  /**
   * @brief Checks if a transition between task groups is required.
   *
   * If a transition is needed, it prepares the necessary sets and flags to handle the transition.
   */
  void TransitionCheck();

  /**
   * @brief Handles the transition between task groups.
   *
   * Stops current tasks, starts target tasks, and updates the current group.
   */
  void Transition();

  // Atomic pointers to manage desired and current task groups safely in a multi-threaded environment

  /**
   * @brief Atomic pointer to store the current task group name.
   *
   * Used to safely update and access the current group in a multi-threaded environment.
   */
  AtomicPtr<std::string> current_group_;
  /**
   * @brief Atomic pointer to store the desired task group name.
   *
   * Used to safely update and access the desired group in a multi-threaded environment.
   */
  AtomicPtr<std::string> desired_group_;

  /**
   * @brief Logger for logging information and errors.
   *
   * Utilizes the spdlog library for logging various events and states within the Executer.
   */
  std::shared_ptr<spdlog::logger> logger_;

  /**
   * @brief Maps to store tasks for resident and standby groups.
   *
   * These unordered maps associate task names with their respective Task instances.
   */
  std::unordered_map<std::string, std::shared_ptr<Task>> resident_group_task_list_;
  std::unordered_map<std::string, std::shared_ptr<Task>> standby_group_task_list_;

  /**
   * @brief Maps to store system settings for tasks.
   *
   * Associates task names with their corresponding SystemSetting configurations.
   */
  std::unordered_map<std::string, SystemSetting> system_setting_map_;

  /**
   * @brief Set of exclusive task groups.
   *
   * Contains the names of task groups that are exclusive, meaning only one can be active at a time.
   */
  std::set<std::string> exclusive_group_set_;

  /**
   * @brief Shared pointer to the NodeMap for retrieving node pointers.
   *
   * Facilitates access to node instances required by tasks.
   */
  std::shared_ptr<NodeMap> node_map_;

  /**
   * @brief Configuration settings for the Executer.
   *
   * Holds all necessary settings required to configure and manage the Executer.
   */
  ExecuterConfig executer_config_;

  /**
   * @brief Sets to manage task and node transitions.
   *
   * These sets keep track of target and current tasks and nodes during transitions.
   */
  std::set<std::shared_ptr<Task>> target_task_set_;
  std::set<std::shared_ptr<Task>> current_task_set_;
  std::set<std::string> target_node_set_;
  std::set<std::string> current_node_set_;
  std::set<std::string> enter_node_set_;
  std::set<std::string> exit_node_set_;

  /**
   * @brief Flags to track the status of node checks and task transitions.
   *
   * These boolean flags indicate the progress and state of transitions.
   */
  bool all_node_exit_check_;
  bool all_node_enter_check_;
  bool task_stop_flag_;
  bool task_start_flag_;
  bool all_current_task_stop_;

  /**
   * @brief Strings to track target and desired task groups.
   *
   * Used to compare and manage transitions between different task groups.
   */
  std::string target_group_;
  std::string desired_group_history_;

  /**
   * @brief Flag indicating whether a transition is in progress.
   *
   * Ensures that only one transition occurs at a time.
   */
  bool is_transition_;

  /**
   * @brief Shared memory topic for desired group.
   */
  std::shared_ptr<SharedMemoryTopic> desired_group_topic_;

  /**
   * @brief Name of the desired group topic.
   */
  std::string desired_group_topic_name_;
};

}  // namespace openrobot::ocm
