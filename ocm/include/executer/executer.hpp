// Start of Selection

#pragma once

#include <log_anywhere/log_anywhere.hpp>
#include <memory>
#include <set>
#include <string>
#include "common/struct_type.hpp"
#include "node/node_map.hpp"
#include "ocm/atomic_ptr.hpp"
#include "ocm/shared_memory_topic.hpp"
#include "task/task.hpp"

namespace openrobot::ocm {

/**
 * @class Executer
 * @brief Manages the execution and transition of task groups within the OpenRobot Operation Control Module (OCM).
 * @brief 管理OpenRobot操作控制模块（OCM）内任务组的执行和切换。
 *
 * The Executer class inherits from TaskBase and is responsible for creating, initializing,
 * and transitioning between different task groups based on the provided configuration.
 * It ensures that tasks are started and stopped appropriately, handling dependencies and
 * exclusive task groups.
 *
 * Executer 类继承自 TaskBase，负责根据提供的配置创建、初始化以及在不同任务组之间进行切换。
 * 它确保任务能够适当地启动和停止，处理依赖关系以及排他性任务组。
 */
class Executer : public TaskBase {
 public:
  /**
   * @brief Constructor for Executer.
   * @brief Executer 的构造函数。
   *
   * Initializes the Executer with its configuration, sets up logging, and initializes internal state variables.
   * 初始化 Executer，设置其配置，建立日志记录，并初始化内部状态变量。
   *
   * @param executer_config Configuration settings for the Executer.
   * @param executer_config Executer 的配置设置。
   * @param node_map Shared pointer to the NodeMap for retrieving node pointers.
   * @param node_map 用于检索节点指针的 NodeMap 的共享指针。
   * @param desired_group_topic_name Name of the desired group topic.
   * @param desired_group_topic_name 期望的任务组主题名称。
   */
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map, const std::string& desired_group_topic_name);

  /**
   * @brief Default destructor.
   * @brief 默认析构函数。
   *
   * Cleans up resources used by the Executer.
   * 清理 Executer 使用的资源。
   */
  ~Executer() = default;

  /**
   * @brief Creates tasks for both resident and standby groups based on the configuration.
   * @brief 根据配置为常驻和待命组创建任务。
   *
   * Retrieves node pointers from the NodeMap and initializes Task instances for each task in the configuration.
   * Also populates the set of exclusive task groups.
   * 从 NodeMap 中检索节点指针，并为配置中的每个任务初始化 Task 实例。
   * 同时填充排他性任务组集合。
   */
  void CreateTask();

  /**
   * @brief Initializes all tasks in the resident group, ensuring that pre-requisite nodes are ready.
   * @brief 初始化常驻组中的所有任务，确保前置节点已准备就绪。
   *
   * Waits until all tasks are ready to start based on their pre-node dependencies.
   * 根据其前置节点依赖关系，等待所有任务准备好启动。
   */
  void InitTask();

  /**
   * @brief Executes the main run loop for the Executer.
   * @brief 执行 Executer 的主运行循环。
   *
   * Checks for transitions between task groups and handles them accordingly.
   * 检查任务组之间的切换并相应地处理。
   */
  void Run() override;

  /**
   * @brief Exits all tasks gracefully.
   * @brief 优雅地退出所有任务。
   *
   * Stops and destroys all tasks in both resident and standby groups, ensuring a clean shutdown.
   * 停止并销毁常驻和待命组中的所有任务，确保干净的关闭。
   */
  void ExitAllTask();

 private:
  /**
   * @brief Checks if a transition between task groups is required.
   * @brief 检查是否需要在任务组之间进行切换。
   *
   * If a transition is needed, it prepares the necessary sets and flags to handle the transition.
   * 如果需要切换，它将准备必要的集合和标志以处理切换。
   */
  void TransitionCheck();

  /**
   * @brief Handles the transition between task groups.
   * @brief 处理任务组之间的切换。
   *
   * Stops current tasks, starts target tasks, and updates the current group.
   * 停止当前任务，启动目标任务，并更新当前任务组。
   */
  void Transition();

  // Atomic pointers to manage desired and current task groups safely in a multi-threaded environment
  // 原子指针用于在多线程环境中安全管理期望和当前的任务组

  /**
   * @brief Atomic pointer to store the current task group name.
   * @brief 原子指针，用于存储当前任务组的名称。
   *
   * Used to safely update and access the current group in a multi-threaded environment.
   * 用于在多线程环境中安全地更新和访问当前组。
   */
  AtomicPtr<std::string> current_group_;

  /**
   * @brief Atomic pointer to store the desired task group name.
   * @brief 原子指针，用于存储期望任务组的名称。
   *
   * Used to safely update and access the desired group in a multi-threaded environment.
   * 用于在多线程环境中安全地更新和访问期望组。
   */
  AtomicPtr<std::string> desired_group_;

  /**
   * @brief Logger for logging information and errors.
   * @brief 用于记录信息和错误的日志记录器。
   *
   * Utilizes the spdlog library for logging various events and states within the Executer.
   * 利用 spdlog 库记录 Executer 内的各种事件和状态。
   */
  std::shared_ptr<spdlog::logger> logger_;

  /**
   * @brief Maps to store tasks for resident and standby groups.
   * @brief 用于存储常驻和待命组任务的映射。
   *
   * These unordered maps associate task names with their respective Task instances.
   * 这些无序映射将任务名称与各自的 Task 实例关联起来。
   */
  std::unordered_map<std::string, std::shared_ptr<Task>> resident_group_task_list_;
  std::unordered_map<std::string, std::shared_ptr<Task>> standby_group_task_list_;

  /**
   * @brief Maps to store system settings for tasks.
   * @brief 用于存储任务系统设置的映射。
   *
   * Associates task names with their corresponding SystemSetting configurations.
   * 将任务名称与其相应的 SystemSetting 配置关联起来。
   */
  std::unordered_map<std::string, SystemSetting> system_setting_map_;

  /**
   * @brief Set of exclusive task groups.
   * @brief 排他性任务组的集合。
   *
   * Contains the names of task groups that are exclusive, meaning only one can be active at a time.
   * 包含排他性任务组的名称，这意味着一次只能有一个任务组处于活动状态。
   */
  std::set<std::string> exclusive_group_set_;

  /**
   * @brief Shared pointer to the NodeMap for retrieving node pointers.
   * @brief 用于检索节点指针的 NodeMap 的共享指针。
   *
   * Facilitates access to node instances required by tasks.
   * 方便访问任务所需的节点实例。
   */
  std::shared_ptr<NodeMap> node_map_;

  /**
   * @brief Configuration settings for the Executer.
   * @brief Executer 的配置设置。
   *
   * Holds all necessary settings required to configure and manage the Executer.
   * 保存配置和管理 Executer 所需的所有必要设置。
   */
  ExecuterConfig executer_config_;

  /**
   * @brief Sets to manage task and node transitions.
   * @brief 用于管理任务和节点切换的集合。
   *
   * These sets keep track of target and current tasks and nodes during transitions.
   * 这些集合在切换期间跟踪目标和当前的任务及节点。
   */
  std::set<std::shared_ptr<Task>> target_task_set_;
  std::set<std::shared_ptr<Task>> current_task_set_;
  std::set<std::string> target_node_set_;
  std::set<std::string> current_node_set_;
  std::set<std::string> enter_node_set_;
  std::set<std::string> exit_node_set_;

  /**
   * @brief Flags to track the status of node checks and task transitions.
   * @brief 用于跟踪节点检查和任务切换状态的标志。
   *
   * These boolean flags indicate the progress and state of transitions.
   * 这些布尔标志指示切换的进展和状态。
   */
  bool all_node_exit_check_;
  bool all_node_enter_check_;
  bool task_stop_flag_;
  bool task_start_flag_;
  bool all_current_task_stop_;

  /**
   * @brief Strings to track target and desired task groups.
   * @brief 用于跟踪目标和期望任务组的字符串。
   *
   * Used to compare and manage transitions between different task groups.
   * 用于比较和管理不同任务组之间的切换。
   */
  std::string target_group_;
  std::string desired_group_history_;

  /**
   * @brief Flag indicating whether a transition is in progress.
   * @brief 指示是否正在进行切换的标志。
   *
   * Ensures that only one transition occurs at a time.
   * 确保一次只发生一次切换。
   */
  bool is_transition_;

  /**
   * @brief Shared memory topic for desired group.
   * @brief 期望任务组的共享内存主题。
   */
  std::shared_ptr<SharedMemoryTopic> desired_group_topic_;

  /**
   * @brief Name of the desired group topic.
   * @brief 期望任务组主题的名称。
   */
  std::string desired_group_topic_name_;
};

}  // namespace openrobot::ocm

// End of Selection
