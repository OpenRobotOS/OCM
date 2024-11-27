

#include "task/task.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a `Task` instance.
 * @brief 构造一个 `Task` 实例。
 *
 * Initializes the TaskBase with the provided settings and sets up node flags based on the task configuration.
 * 使用提供的设置初始化 TaskBase，并根据任务配置设置节点标志。
 * It also initializes the execution period and logs the creation of the task thread.
 * 它还初始化执行周期并记录任务线程的创建。
 *
 * @param task_setting Configuration settings for the task, including task name, timer settings, and node configurations.
 * @param task_setting 任务的配置设置，包括任务名称、计时器设置和节点配置。
 * @param node_list Shared pointer to a vector of node pointers associated with the task.
 * @param node_list 与任务关联的节点指针向量的共享指针。
 * @param all_priority_enable Flag indicating whether all priority settings are enabled for the task.
 * @param all_priority_enable 标志，指示是否启用了任务的所有优先级设置。
 * @param all_cpu_affinity_enable Flag indicating whether all CPU affinity settings are enabled for the task.
 * @param all_cpu_affinity_enable 标志，指示是否启用了任务的所有 CPU 亲和性设置。
 */
Task::Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list, bool all_priority_enable,
           bool all_cpu_affinity_enable)
    : TaskBase(task_setting.task_name, task_setting.timer_setting.timer_type, static_cast<double>(task_setting.launch_setting.delay),
               all_priority_enable, all_cpu_affinity_enable),
      task_setting_(task_setting),
      node_list_(node_list) {
  // Set the task's execution period based on the configuration
  // 根据配置设置任务的执行周期
  SetPeriod(task_setting_.timer_setting.period);

  // Initialize node output and initialization flags
  // 初始化节点输出和初始化标志
  for (const auto& node : task_setting_.node_list) {
    node_output_flag_[node.node_name] = node.output_enable;
    node_init_flag_[node.node_name] = false;
  }
}

/**
 * @brief Initializes all nodes associated with the task.
 * @brief 初始化与任务关联的所有节点。
 *
 * Sets all nodes' initialization flags to true and calls InitNode to perform initialization.
 * 将所有节点的初始化标志设置为真，并调用 InitNode 进行初始化。
 * This ensures that all nodes are properly initialized before the task starts running.
 * 这确保在任务开始运行之前，所有节点都被正确初始化。
 */
void Task::Init() {
  for (auto& node : node_init_flag_) {
    node.second = true;
  }
}

/**
 * @brief Initializes a specific subset of nodes associated with the task.
 * @brief 初始化与任务关联的特定子集节点。
 *
 * @param init_node_list Set of node names to initialize.
 * @param init_node_list 要初始化的节点名称集合。
 * @return Set of node names that were successfully initialized.
 * @return 成功初始化的节点名称集合。
 *
 * This method allows selective initialization of nodes based on the provided list.
 * 该方法允许根据提供的列表选择性地初始化节点。
 * Only nodes present in `init_node_list` will have their initialization flags set to true and be initialized.
 * 只有在 `init_node_list` 中存在的节点其初始化标志才会被设置为真并被初始化。
 */
std::set<std::string> Task::Init(const std::set<std::string>& init_node_list) {
  std::set<std::string> init_node_list_result;
  for (auto& node : node_init_flag_) {
    if (init_node_list.find(node.first) != init_node_list.end()) {
      node.second = true;
      init_node_list_result.insert(node.first);
    }
  }
  return init_node_list_result;
}

/**
 * @brief Executes the task by running and optionally outputting each node.
 * @brief 通过运行并可选地输出每个节点来执行任务。
 *
 * Iterates through all nodes associated with the task. For each node, it checks if the node has been
 * initialized in the current cycle. If not, it runs the node once and outputs its data if enabled.
 * After execution, it resets the node's initialization flag for the next cycle.
 * 遍历与任务关联的所有节点。对于每个节点，它检查该节点是否已在当前周期中初始化。如果没有，它运行该节点一次，并在启用的情况下输出其数据。执行后，它重置该节点的初始化标志以进行下一个周期。
 */
void Task::Run() {
  // Iterate through all nodes associated with the task
  // 遍历与任务关联的所有节点
  for (auto& node : *node_list_) {
    const auto& node_name = node->GetNodeName();
    // If the node hasn't been initialized in this cycle, run it once and optionally output
    // 如果节点在本周期尚未初始化，则运行一次并可选择性地输出
    if (node_init_flag_[node_name]) {
      node->Init();
    } else {
      // Reset the initialization flag for the next cycle
      // 重置下一个周期的初始化标志
      node_init_flag_.at(node_name) = false;
    }
    node->Execute();
    if (node_output_flag_[node_name]) {
      node->Output();
    }
    node->SetState(NodeState::RUNNING);
  }
}

/**
 * @brief Retrieves the task's configuration settings.
 * @brief 获取任务的配置设置。
 *
 * @return Constant reference to the TaskSetting structure.
 * @return TaskSetting 结构的常量引用。
 *
 * Provides access to the task's configuration, including task name, timer settings, and node configurations.
 * 提供对任务配置的访问，包括任务名称、计时器设置和节点配置。
 */
const TaskSetting& Task::GetTaskSetting() const { return task_setting_; }

}  // namespace openrobot::ocm
