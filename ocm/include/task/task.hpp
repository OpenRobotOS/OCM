// Start of Selection
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
 * @brief 表示 OpenRobot OCM（操作控制模块）中的任务。
 *
 * @details
 * Inherits from TaskBase and manages the initialization and execution of nodes associated with the task.
 * 继承自 TaskBase，并管理与任务关联的节点的初始化和执行。
 * Each Task handles a collection of nodes, initializing them based on configuration settings and executing
 * their respective processes during runtime.
 * 每个 Task 处理一组节点，根据配置设置初始化它们，并在运行时执行各自的进程。
 */
class Task : public TaskBase {
 public:
  /**
   * @brief Constructor for Task.
   * @brief Task 的构造函数。
   *
   * @details
   * Initializes the TaskBase with task name, timer type, delay, and sets up node flags.
   * 初始化 TaskBase，包括任务名称、定时器类型、延迟，并设置节点标志。
   *
   * @param task_setting Configuration settings for the task, including task name, timer settings, and node configurations.
   *                     任务的配置设置，包括任务名称、定时器设置和节点配置。
   * @param node_list Shared pointer to a vector of node pointers associated with the task.
   *                  关联任务的节点指针向量的共享指针。
   * @param all_priority_enable Flag indicating whether all priority settings are enabled for the task.
   *                             标志，指示是否为任务启用所有优先级设置。
   * @param all_cpu_affinity_enable Flag indicating whether all CPU affinity settings are enabled for the task.
   *                                 标志，指示是否为任务启用所有 CPU 亲和性设置。
   */
  Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
       bool all_cpu_affinity_enable);

  /**
   * @brief Default destructor.
   * @brief 默认析构函数。
   *
   * @details
   * Cleans up any resources allocated by the Task.
   * 清理 Task 分配的所有资源。
   */
  ~Task() = default;

  /**
   * @brief Initializes all nodes associated with the task.
   * @brief 初始化与任务关联的所有节点。
   *
   * @details
   * Sets all nodes' initialization flags to true and calls InitNode to perform initialization.
   * 将所有节点的初始化标志设置为 true 并调用 InitNode 执行初始化。
   * This method ensures that all nodes are properly set up before the task begins execution.
   * 该方法确保在任务开始执行之前所有节点都已正确设置。
   */
  void Init();

  /**
   * @brief Initializes a specific subset of nodes associated with the task.
   * @brief 初始化与任务关联的特定子集节点。
   *
   * @details
   * This method allows selective initialization of nodes based on the provided list.
   * 根据提供的列表选择性地初始化节点。
   * Only nodes present in `init_node_list` will be initialized.
   * 仅初始化 `init_node_list` 中存在的节点。
   *
   * @param init_node_list Set of node names to initialize.
   *                       要初始化的节点名称集合。
   * @return Set of node names that were successfully initialized.
   *         成功初始化的节点名称集合。
   */
  std::set<std::string> Init(const std::set<std::string>& init_node_list);

  /**
   * @brief Executes the task by running and optionally outputting each node.
   * @brief 通过运行并可选择性地输出每个节点来执行任务。
   *
   * @details
   * This method overrides the Run method from TaskBase and is called periodically based on the task's timer.
   * 该方法重写了 TaskBase 的 Run 方法，并根据任务的定时器定期调用。
   * It iterates through all associated nodes, running them and handling their output if enabled.
   * 它遍历所有关联的节点，运行它们并在启用时处理它们的输出。
   */
  void Run() override;

  /**
   * @brief Retrieves the task's configuration settings.
   * @brief 获取任务的配置设置。
   *
   * @details
   * Provides access to the task's configuration, including task name, timer settings, and node configurations.
   * 提供对任务配置的访问，包括任务名称、定时器设置和节点配置。
   *
   * @return Constant reference to the TaskSetting structure.
   *         对 TaskSetting 结构的常量引用。
   */
  const TaskSetting& GetTaskSetting() const;

 private:
  /**
   * @brief Initializes nodes based on their initialization flags.
   * @brief 根据节点的初始化标志初始化节点。
   *
   * @details
   * This method iterates through the node list and initializes each node that has its initialization flag set to true.
   * 该方法遍历节点列表，初始化每个初始化标志设置为 true 的节点。
   * It calls the Init and RunOnce methods on each node and handles their output if enabled.
   * 它调用每个节点的 Init 和 RunOnce 方法，并在启用时处理它们的输出。
   *
   * @param node_init_flag Map containing node names and their initialization status.
   *                       包含节点名称及其初始化状态的映射。
   */
  void InitNode(const std::unordered_map<std::string, bool>& node_init_flag);

  /**
   * @brief Map to track whether each node should output data.
   * @brief 用于跟踪每个节点是否应输出数据的映射。
   *
   * @details
   * The key is the node's name, and the value indicates whether output is enabled for that node.
   * 键是节点的名称，值指示该节点是否启用了输出。
   */
  std::unordered_map<std::string, bool> node_output_flag_;

  /**
   * @brief Configuration settings for the task.
   * @brief 任务的配置设置。
   *
   * @details
   * Contains all relevant settings such as task name, timer settings, and node configurations.
   * 包含所有相关的设置，如任务名称、定时器设置和节点配置。
   */
  TaskSetting task_setting_;

  /**
   * @brief Map to track whether each node has been initialized.
   * @brief 用于跟踪每个节点是否已初始化的映射。
   *
   * @details
   * The key is the node's name, and the value indicates whether the node has been initialized.
   * 键是节点的名称，值指示该节点是否已初始化。
   */
  std::unordered_map<std::string, bool> node_init_flag_;

  /**
   * @brief Shared pointer to the list of nodes associated with the task.
   * @brief 关联任务的节点列表的共享指针。
   *
   * @details
   * Each node in the list represents a distinct component or process that the task manages and executes.
   * 列表中的每个节点代表任务管理和执行的不同组件或进程。
   */
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};

}  // namespace openrobot::ocm
// End of Selection