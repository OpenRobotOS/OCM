// executer.cpp
#include "executer/executer.hpp"

#include <algorithm>
#include <chrono>
#include <thread>
#include "executer/desired_group_data.hpp"

namespace openrobot::ocm {

// Constructor Implementation
/**
 * @brief Constructs an Executer instance with the given configuration and node map.
 *
 * Initializes the base TaskBase class, sets up the logger, configures the timer period,
 * and starts the task using the provided system settings.
 *
 * @param executer_config Configuration settings for the Executer.
 * @param node_map Shared pointer to the NodeMap for retrieving node pointers.
 */
Executer::Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map, const std::string& desired_group_topic_name)
    : TaskBase(executer_config.executer_setting.package_name, executer_config.executer_setting.timer_setting.timer_type, 0.0,
               executer_config.executer_setting.all_priority_enable, executer_config.executer_setting.all_cpu_affinity_enable),
      node_map_(node_map),
      executer_config_(executer_config),
      desired_group_("empty_init"),
      current_group_("empty_init"),
      desired_group_history_("empty_init"),
      target_group_("empty_init"),
      is_transition_(false),
      all_node_exit_check_(false),
      all_node_enter_check_(false),
      task_stop_flag_(true),
      task_start_flag_(true),
      all_current_task_stop_(false),
      desired_group_topic_name_(desired_group_topic_name) {
  // Initialize logger
  logger_ = GetLogger();

  // Initialize desired group topic
  desired_group_topic_ = std::make_shared<SharedMemoryTopic>();

  // Set the timer period based on the executer configuration
  SetPeriod(executer_config_.executer_setting.timer_setting.period);

  // Start the task with the specified system settings
  TaskStart(executer_config_.executer_setting.system_setting);
}

/**
 * @brief Exits all tasks gracefully by stopping and destroying them.
 *
 * Iterates through both resident and standby task lists, stopping each task with idle system settings
 * and then destroying the task instance. Introduces a brief sleep to ensure all tasks have been
 * properly terminated.
 */
void Executer::ExitAllTask() {
  // Stop and destroy all tasks in the resident group
  for (auto& task : resident_group_task_list_) {
    task.second->TaskStop(executer_config_.executer_setting.idle_system_setting);
    task.second->TaskDestroy();
  }

  // Stop and destroy all tasks in the standby group
  for (auto& task : standby_group_task_list_) {
    task.second->TaskStop(executer_config_.executer_setting.idle_system_setting);
    task.second->TaskDestroy();
  }

  // Sleep briefly to ensure all tasks have been properly terminated
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

// CreateTask Method Implementation
/**
 * @brief Creates tasks for both resident and standby groups based on the configuration.
 *
 * For each task in the resident and standby groups, retrieves the associated nodes from the NodeMap,
 * initializes a Task instance, and adds it to the respective task list. Also logs the addition
 * of each task and populates the set of exclusive task groups.
 */
void Executer::CreateTask() {
  // Create tasks for the resident group
  for (auto& task_setting : executer_config_.task_list.resident_group) {
    // Create a shared pointer to a vector of NodeBase pointers
    std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();

    // Retrieve and add each node to the node list
    for (auto& node_config : task_setting.second.node_list) {
      node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
    }

    // Initialize the Task instance and add it to the resident group task list
    resident_group_task_list_[task_setting.second.task_name] =
        std::make_shared<Task>(task_setting.second, node_list, executer_config_.executer_setting.all_priority_enable,
                               executer_config_.executer_setting.all_cpu_affinity_enable);

    // Log the addition of the task
    logger_->debug("Executer: Task {} added.", task_setting.second.task_name);
  }

  // Create tasks for the standby group
  for (auto& task_setting : executer_config_.task_list.standby_group) {
    // Create a shared pointer to a vector of NodeBase pointers
    std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();

    // Retrieve and add each node to the node list
    for (auto& node_config : task_setting.second.node_list) {
      node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
    }

    // Initialize the Task instance and add it to the standby group task list
    standby_group_task_list_[task_setting.second.task_name] =
        std::make_shared<Task>(task_setting.second, node_list, executer_config_.executer_setting.all_priority_enable,
                               executer_config_.executer_setting.all_cpu_affinity_enable);

    // Log the addition of the task
    logger_->debug("Executer: Task {} added.", task_setting.second.task_name);
  }

  // Add exclusive task groups to the set
  for (auto& exclusive_task_group : executer_config_.exclusive_task_group) {
    logger_->debug("Executer: Exclusive group {} added.", exclusive_task_group.second.group_name);
    exclusive_group_set_.insert(exclusive_task_group.second.group_name);
  }
}

// InitTask Method Implementation
/**
 * @brief Initializes all tasks in the resident group, ensuring that pre-requisite nodes are ready.
 *
 * For each task in the resident group, checks if all pre-nodes are running or if there are no pre-nodes.
 * If the conditions are met, initializes and starts the task. Continues this process until all tasks
 * have been successfully started.
 */
void Executer::InitTask() {
  // Vector to hold tasks waiting to start, along with a flag indicating if they've been started
  std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
  // Set to keep track of task names waiting to start
  std::set<std::string> task_set_wait_to_start;

  // Prepare the list of tasks waiting to start
  for (auto& task_ptr : resident_group_task_list_) {
    task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr.second));
    task_set_wait_to_start.insert(task_ptr.second->GetTaskName());
  }

  // Continuously check if tasks can be started
  while (!task_set_wait_to_start.empty()) {
    for (auto& task : task_list_wait_to_start) {
      if (!task.first) {
        bool is_pre_node_empty = task.second->GetTaskSetting().launch_setting.pre_node.empty();
        const auto& pre_node_list = task.second->GetTaskSetting().launch_setting.pre_node;

        // Check if all pre-nodes are running
        bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
          return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
        });

        // If pre-nodes are ready or there are no pre-nodes, initialize and start the task
        if (is_pre_node_empty || is_pre_node_ready) {
          task.first = true;                                                      // Mark task as started
          task.second->Init();                                                    // Initialize the task
          task.second->TaskStart(task.second->GetTaskSetting().system_setting);   // Start the task
          task_set_wait_to_start.erase(task.second->GetTaskName());               // Remove from waiting set
          logger_->info("Executer: Task {} start.", task.second->GetTaskName());  // Log the start
        }
      }
    }
    // Sleep briefly to prevent busy-waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

// Run Method Implementation
/**
 * @brief Executes the main run loop for the Executer.
 *
 * This method is called periodically based on the configured timer period. It checks for any
 * required transitions between task groups and handles them accordingly.
 */
void Executer::Run() {
  desired_group_topic_->SubscribeNoWait<DesiredGroupData>(
      desired_group_topic_name_, desired_group_topic_name_,
      [this](const DesiredGroupData& desired_group) { desired_group_ = desired_group.desired_group; });
  // Check if a transition is required
  TransitionCheck();

  // If a transition is in progress, handle it
  if (is_transition_) {
    Transition();
  }
}

// TransitionCheck Method Implementation
/**
 * @brief Checks if a transition between task groups is required.
 *
 * Determines whether the desired task group differs from the current group. If a transition is needed
 * and the desired group is an exclusive group, prepares the necessary sets and flags to handle the transition.
 * Logs an error if the desired group is not exclusive.
 */
void Executer::TransitionCheck() {
  if (!is_transition_) {
    const auto& desired_group = desired_group_.GetValue();
    const auto& current_group = current_group_.GetValue();

    // Check if the desired group is different from the current group
    if (desired_group != current_group) {
      // Check if the desired group is an exclusive group
      if (exclusive_group_set_.find(desired_group) != exclusive_group_set_.end()) {
        // Clear all existing task and node sets
        target_task_set_.clear();
        current_task_set_.clear();
        target_node_set_.clear();
        current_node_set_.clear();
        enter_node_set_.clear();
        exit_node_set_.clear();

        // Populate target task and node sets based on the desired group
        const auto& exclusive_group_config = executer_config_.exclusive_task_group;
        for (auto& task_config : exclusive_group_config.at(desired_group).task_list) {
          const auto& task_name = task_config.second.task_name;
          target_task_set_.insert(standby_group_task_list_.at(task_name));
          for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
            target_node_set_.insert(node.node_name);
          }
        }

        // If there's a current group, populate the current task and node sets
        if (current_group != "empty_init") {
          for (auto& task_config : exclusive_group_config.at(current_group).task_list) {
            const auto& task_name = task_config.second.task_name;
            current_task_set_.insert(standby_group_task_list_.at(task_name));
            for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
              current_node_set_.insert(node.node_name);
            }
          }
        }

        // Determine which nodes need to exit and which need to enter
        std::set_difference(current_node_set_.begin(), current_node_set_.end(), target_node_set_.begin(), target_node_set_.end(),
                            std::inserter(exit_node_set_, exit_node_set_.begin()));

        std::set_difference(target_node_set_.begin(), target_node_set_.end(), current_node_set_.begin(), current_node_set_.end(),
                            std::inserter(enter_node_set_, enter_node_set_.begin()));

        // Reset transition-related flags
        all_node_exit_check_ = false;
        all_node_enter_check_ = false;
        is_transition_ = true;
        task_stop_flag_ = true;
        task_start_flag_ = true;
        all_current_task_stop_ = false;
        target_group_ = desired_group;

        // Log the transition initiation
        logger_->info("Executer: Transition from group {} to group {}", ColorPrint(current_group, ColorEnum::YELLOW),
                      ColorPrint(desired_group, ColorEnum::YELLOW));
      } else {
        // If the desired group is not exclusive, log an error
        if (desired_group_history_ != desired_group) {
          desired_group_history_ = desired_group;
          logger_->error("Executer: Target group {} is not an exclusive group.", ColorPrint(desired_group, ColorEnum::RED));
        }
      }
    }
  }
}

// Transition Method Implementation
/**
 * @brief Handles the transition between task groups.
 *
 * Manages the stopping of current tasks, starting of target tasks, and updating of the current group.
 * Ensures that all nodes have exited and entered as required before completing the transition.
 * Logs the transition details and updates internal state flags accordingly.
 */
void Executer::Transition() {
  if (all_node_exit_check_ && all_node_enter_check_) {
    if (task_stop_flag_) {
      // Stop all current tasks
      task_stop_flag_ = false;
      for (auto& task : current_task_set_) {
        task->TaskStop(executer_config_.executer_setting.idle_system_setting);
      }
    }

    if (all_current_task_stop_) {
      // Prepare to start target tasks
      std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
      std::set<std::string> task_set_wait_to_start;
      for (auto& task_ptr : target_task_set_) {
        task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr));
        task_set_wait_to_start.insert(task_ptr->GetTaskName());
      }
      std::set<std::string> all_init_node_set_log;

      // Initialize and start target tasks
      while (!task_set_wait_to_start.empty()) {
        for (auto& task : task_list_wait_to_start) {
          const auto& task_name = task.second->GetTaskName();
          if (!task.first) {
            const auto& task_setting = executer_config_.exclusive_task_group.at(target_group_).task_list.at(task_name);
            bool is_pre_node_empty = task_setting.pre_node.empty();
            const auto& pre_node_list = task_setting.pre_node;

            // Combine forced init nodes and entering nodes
            std::set<std::string> force_init_node_set(task_setting.force_init_node.begin(), task_setting.force_init_node.end());
            std::set<std::string> all_init_node_set;
            std::set_union(force_init_node_set.begin(), force_init_node_set.end(), enter_node_set_.begin(), enter_node_set_.end(),
                           std::inserter(all_init_node_set, all_init_node_set.begin()));

            // Check if all pre-nodes are ready
            bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
              return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
            });

            // If pre-nodes are ready or there are no pre-nodes, initialize and start the task
            if (is_pre_node_empty || is_pre_node_ready) {
              task.first = true;                                                     // Mark task as started
              auto init_node_set = task.second->Init(all_init_node_set);             // Initialize with required nodes
              task.second->TaskStart(task.second->GetTaskSetting().system_setting);  // Start the task
              all_init_node_set_log.insert(init_node_set.begin(), init_node_set.end());
              task_set_wait_to_start.erase(task.second->GetTaskName());               // Remove from waiting set
              logger_->info("Executer: Task {} start.", task.second->GetTaskName());  // Log the start
            }
          }
        }
        // Sleep briefly to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }

      // Update the current group and reset transition flags
      current_group_ = target_group_;
      is_transition_ = false;

      // Log the completion of the transition
      logger_->info("Executer: Exit node: {}, Enter node: {}", JointStrSet(exit_node_set_, ","), JointStrSet(enter_node_set_, ","));
      logger_->info("Executer: Transition to group {} finished.", ColorPrint(target_group_, ColorEnum::YELLOW));
    } else {
      // Check if all current tasks have stopped
      all_current_task_stop_ =
          std::all_of(current_task_set_.begin(), current_task_set_.end(), [](const auto& task) { return task->GetState() == TaskState::STANDBY; });
    }
  } else {
    // Check if all exit and enter nodes have completed their checks
    all_node_exit_check_ =
        std::all_of(exit_node_set_.begin(), exit_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->ExitCheck(); });
    all_node_enter_check_ =
        std::all_of(enter_node_set_.begin(), enter_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->EnterCheck(); });
  }
}

}  // namespace openrobot::ocm