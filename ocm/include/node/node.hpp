// node_base.hpp
#pragma once

#include <atomic>
#include <string>
#include "common/enum.hpp"

namespace openrobot::ocm {

/**
 * @class NodeBase
 * @brief Abstract base class representing a generic node in the OpenRobot OCM.
 *
 * The `NodeBase` class serves as an interface for all node types within the OpenRobot
 * Operation Control Module (OCM). It defines the essential lifecycle methods that
 * each node must implement, such as initialization, execution, and state transitions.
 * This class ensures that all derived nodes adhere to a consistent interface, enabling
 * seamless integration and management within the OCM framework.
 *
 * @note This class is abstract and cannot be instantiated directly. It must be inherited
 *       by concrete node implementations that provide definitions for the pure virtual methods.
 */
class NodeBase {
 public:
  /**
   * @brief Constructs a new NodeBase instance with the given node name.
   *
   * Initializes the node's name and sets its initial state to `NodeState::INIT`.
   *
   * @param node_name The unique name identifier for the node.
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
   *
   * Prevents copying of `NodeBase` instances to maintain unique ownership semantics.
   */
  NodeBase& operator=(const NodeBase&) = delete;

  /**
   * @brief Deleted move constructor.
   *
   * Prevents moving of `NodeBase` instances to maintain unique ownership semantics.
   */
  NodeBase(NodeBase&&) = delete;

  /**
   * @brief Virtual destructor.
   *
   * Ensures that derived class destructors are called correctly when deleting instances
   * through base class pointers.
   */
  virtual ~NodeBase() = default;

  /**
   * @brief Initializes the node.
   *
   * Pure virtual method that must be overridden by derived classes to perform
   * any necessary initialization steps for the node.
   *
   * @note This method is called before the node begins execution.
   */
  virtual void Init() = 0;

  /**
   * @brief Executes the node's primary functionality.
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the main execution logic of the node.
   *
   * @note This method is intended to be called periodically or in response to events.
   */
  virtual void Execute() = 0;

  /**
   * @brief Outputs the node's state or data.
   *
   * Pure virtual method that must be overridden by derived classes to handle
   * any output operations, such as sending data to other components or logging.
   */
  virtual void Output() = 0;

  /**
   * @brief Attempts to enter the node's active state.
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the conditions under which the node can transition to its active state.
   *
   * @return `true` if the node successfully enters the active state; otherwise, `false`.
   */
  virtual bool TryEnter() = 0;

  /**
   * @brief Attempts to exit the node's active state.
   *
   * Pure virtual method that must be overridden by derived classes to define
   * the conditions under which the node can transition out of its active state.
   *
   * @return `true` if the node successfully exits the active state; otherwise, `false`.
   */
  virtual bool TryExit() = 0;

  /**
   * @brief Executes the node's functionality once and updates its state.
   *
   * Calls the `Execute` method and sets the node's state to `NodeState::RUNNING`.
   *
   * @note This method encapsulates a single execution cycle of the node.
   */
  void RunOnce() {
    Execute();
    SetState(NodeState::RUNNING);
  }

  /**
   * @brief Checks if the node can enter the active state.
   *
   * Invokes the `TryEnter` method to determine if the node meets the criteria
   * to transition to its active state.
   *
   * @return `true` if the node can enter the active state; otherwise, `false`.
   */
  bool EnterCheck() { return TryEnter(); }

  /**
   * @brief Checks if the node can exit the active state.
   *
   * Sets the node's state to `NodeState::STANDBY` and invokes the `TryExit` method
   * to determine if the node meets the criteria to transition out of its active state.
   *
   * @return `true` if the node can exit the active state; otherwise, `false`.
   */
  bool ExitCheck() {
    SetState(NodeState::STANDBY);
    return TryExit();
  }

  /**
   * @brief Retrieves the current state of the node.
   *
   * @return The current `NodeState` of the node.
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
   *
   * @return A constant reference to the node's name string.
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
   *
   * Updates the node's current state to the specified `NodeState`.
   *
   * @param state The new state to set for the node.
   */
  void SetState(NodeState state) { state_.store(state); }

  std::string node_name_;        /**< The unique name identifier of the node */
  std::atomic<NodeState> state_; /**< The current state of the node, managed atomically for thread safety */
};

}  // namespace openrobot::ocm
