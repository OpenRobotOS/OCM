
#pragma once

#include <atomic>
#include <string>
#include "common/enum.hpp"

namespace openrobot::ocm {

/**
 * @class NodeBase
 * @brief Abstract base class representing a generic node in the OpenRobot OCM.
 *        抽象基类，表示OpenRobot OCM中的通用节点。
 *
 * The `NodeBase` class serves as an interface for all node types within the OpenRobot
 * Operation Control Module (OCM). It defines the essential lifecycle methods that
 * each node must implement, such as initialization, execution, and state transitions.
 * This class ensures that all derived nodes adhere to a consistent interface, enabling
 * seamless integration and management within the OCM framework.
 *
 * `NodeBase` 类作为OpenRobot操作控制模块（OCM）中所有节点类型的接口。它定义了每个节点必须实现的基本生命周期方法，如初始化、执行和状态转换。
 * 该类确保所有派生节点遵循一致的接口，从而实现OCM框架内的无缝集成和管理。
 *
 * @note This class is abstract and cannot be instantiated directly. It must be inherited
 *       by concrete node implementations that provide definitions for the pure virtual methods.
 * @note 此类是抽象的，不能被直接实例化。必须由具体的节点实现继承，并为纯虚方法提供定义。
 */
class NodeBase {
 public:
  /**
   * @brief Constructs a new NodeBase instance with the given node name.
   *        使用给定的节点名称构造一个新的 NodeBase 实例。
   *
   * Initializes the node's name and sets its initial state to `NodeState::INIT`.
   *
   * 初始化节点的名称并将其初始状态设置为 `NodeState::INIT`。
   *
   * @param node_name The unique name identifier for the node.
   *                  节点的唯一名称标识符。
   *
   * @example
   * @code
   * class ConcreteNode : public NodeBase {
   * public:
   *     ConcreteNode(const std::string& name) : NodeBase(name) {}
   *     void Init() override { /* Initialization code * / }
   *     void Execute() override { /* Execution code * / }
   *     void Output() override { /* Output code * / }
   *     bool TryEnter() override { /* Enter logic * / }
   *     bool TryExit() override { /* Exit logic * / }
   * };
   *
   * ConcreteNode node("SensorNode");
   * node.Init();
   * node.Execute();
   * @endcode
   */
  NodeBase(const std::string& node_name) : node_name_(node_name) { state_.store(NodeState::INIT); }

  /**
   * @brief Deleted copy assignment operator.
   *        删除的拷贝赋值操作符。
   *
   * Prevents copying of `NodeBase` instances to maintain unique ownership semantics.
   *
   * 防止拷贝 `NodeBase` 实例以保持唯一所有权语义。
   */
  NodeBase& operator=(const NodeBase&) = delete;

  /**
   * @brief Deleted move constructor.
   *        删除的移动构造函数。
   *
   * Prevents moving of `NodeBase` instances to maintain unique ownership semantics.
   *
   * 防止移动 `NodeBase` 实例以保持唯一所有权语义。
   */
  NodeBase(NodeBase&&) = delete;

  /**
   * @brief Virtual destructor.
   *        虚析构函数。
   *
   * Ensures that derived class destructors are called correctly when deleting instances
   * through base class pointers.
   *
   * 确保通过基类指针删除实例时，派生类的析构函数被正确调用。
   */
  virtual ~NodeBase() = default;

  /**
   * @brief Initializes the node.
   *        初始化节点。
   *
   * Pure virtual method that must be overridden by derived classes to perform
   * any necessary initialization steps for the node.
   *
   * 纯虚方法，必须由派生类重写以执行节点的任何必要初始化步骤。
   *
   * @note This method is called before the node begins execution.
   * @note 此方法在节点开始执行之前调用。
   */
  virtual void Init() = 0;

  /**
   * @brief Executes the node's primary functionality.
   *        执行节点的主要功能。
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the main execution logic of the node.
   *
   * 纯虚方法，必须由派生类重写以定义节点的主要执行逻辑。
   *
   * @note This method is intended to be called periodically or in response to events.
   * @note 此方法旨在定期调用或响应事件调用。
   */
  virtual void Execute() = 0;

  /**
   * @brief Outputs the node's state or data.
   *        输出节点的状态或数据。
   *
   * Pure virtual method that must be overridden by derived classes to handle
   * any output operations, such as sending data to other components or logging.
   *
   * 纯虚方法，必须由派生类重写以处理任何输出操作，例如向其他组件发送数据或记录日志。
   */
  virtual void Output() = 0;

  /**
   * @brief Attempts to enter the node's active state.
   *        尝试进入节点的活动状态。
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the conditions under which the node can transition to its active state.
   *
   * 纯虚方法，必须由派生类重写以定义节点可以过渡到活动状态的条件。
   *
   * @return `true` if the node successfully enters the active state; otherwise, `false`.
   *         如果节点成功进入活动状态，则返回 `true`；否则，返回 `false`。
   */
  virtual bool TryEnter() = 0;

  /**
   * @brief Attempts to exit the node's active state.
   *        尝试退出节点的活动状态。
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the conditions under which the node can transition out of its active state.
   *
   * 纯虚方法，必须由派生类重写以定义节点可以过渡出活动状态的条件。
   *
   * @return `true` if the node successfully exits the active state; otherwise, `false`.
   *         如果节点成功退出活动状态，则返回 `true`；否则，返回 `false`。
   */
  virtual bool TryExit() = 0;

  /**
   * @brief Executes the node's functionality once and updates its state.
   *        执行节点的功能一次并更新其状态。
   *
   * Calls the `Execute` method and sets the node's state to `NodeState::RUNNING`.
   *
   * 调用 `Execute` 方法并将节点的状态设置为 `NodeState::RUNNING`。
   *
   * @note This method encapsulates a single execution cycle of the node.
   * @note 此方法封装了节点的单个执行周期。
   */
  void RunOnce() {
    Execute();
    SetState(NodeState::RUNNING);
  }

  /**
   * @brief Checks if the node can enter the active state.
   *        检查节点是否可以进入活动状态。
   *
   * Invokes the `TryEnter` method to determine if the node meets the criteria
   * to transition to its active state.
   *
   * 调用 `TryEnter` 方法以确定节点是否满足过渡到活动状态的条件。
   *
   * @return `true` if the node can enter the active state; otherwise, `false`.
   *         如果节点可以进入活动状态，则返回 `true`；否则，返回 `false`。
   */
  bool EnterCheck() { return TryEnter(); }

  /**
   * @brief Checks if the node can exit the active state.
   *        检查节点是否可以退出活动状态。
   *
   * Sets the node's state to `NodeState::STANDBY` and invokes the `TryExit` method
   * to determine if the node meets the criteria to transition out of its active state.
   *
   * 将节点的状态设置为 `NodeState::STANDBY` 并调用 `TryExit` 方法以确定节点是否满足过渡出活动状态的条件。
   *
   * @return `true` if the node can exit the active state; otherwise, `false`.
   *         如果节点可以退出活动状态，则返回 `true`；否则，返回 `false`。
   */
  bool ExitCheck() {
    SetState(NodeState::STANDBY);
    return TryExit();
  }

  /**
   * @brief Retrieves the current state of the node.
   *        获取节点的当前状态。
   *
   * @return The current `NodeState` of the node.
   *         节点当前的 `NodeState`。
   *
   * @example
   * @code
   * NodeState current_state = node.GetState();
   * if (current_state == NodeState::RUNNING) {
   *     // Perform actions based on the running state
   * }
   * @endcode
   */
  NodeState GetState() const { return state_.load(); }

  /**
   * @brief Retrieves the name of the node.
   *        获取节点的名称。
   *
   * @return A constant reference to the node's name string.
   *         节点名称字符串的常量引用。
   *
   * @example
   * @code
   * std::string name = node.GetNodeName();
   * std::cout << "Node Name: " << name << std::endl;
   * @endcode
   */
  const std::string& GetNodeName() const { return node_name_; }

 private:
  /**
   * @brief Sets the state of the node.
   *        设置节点的状态。
   *
   * Updates the node's current state to the specified `NodeState`.
   *
   * 将节点的当前状态更新为指定的 `NodeState`。
   *
   * @param state The new state to set for the node.
   *              要为节点设置的新状态。
   */
  void SetState(NodeState state) { state_.store(state); }

  std::string node_name_;        /**< The unique name identifier of the node
                                   节点的唯一名称标识符 */
  std::atomic<NodeState> state_; /**< The current state of the node, managed atomically for thread safety
                                   节点的当前状态，通过原子操作管理以确保线程安全 */
};

}  // namespace openrobot::ocm
