// node_map.hpp
#pragma once

#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include "node/node.hpp"

namespace openrobot::ocm {

/**
 * @class NodeMap
 * @brief Manages a collection of nodes, allowing addition and retrieval of nodes by name.
 *
 * The `NodeMap` class provides functionality to store and manage nodes identified by unique
 * string names. It ensures that each node name is unique within the map and provides methods
 * to add new nodes and retrieve existing nodes. Attempting to add a node with a duplicate name
 * or retrieving a non-existent node will result in an exception.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class NodeMap final {
 public:
  /**
   * @brief Default constructor.
   *
   * Initializes an empty `NodeMap`.
   */
  NodeMap() = default;

  /**
   * @brief Default destructor.
   *
   * Cleans up the `NodeMap`. As all managed nodes are handled via `std::shared_ptr`,
   * explicit cleanup is not required.
   */
  ~NodeMap() = default;

  /**
   * @brief Adds a new node to the map.
   *
   * This method inserts a new node into the `NodeMap` with the specified name and pointer.
   * If a node with the given name already exists, the method throws a `std::runtime_error`.
   *
   * @param node_name The unique name identifier for the node.
   * @param node_ptr A `std::shared_ptr` to the node to be added.
   *
   * @throws std::runtime_error If a node with the given name already exists in the map.
   *
   * @example
   * @code
   * std::shared_ptr<NodeBase> node = std::make_shared<ConcreteNode>();
   * node_map.AddNode("Node1", node);
   * @endcode
   */
  void AddNode(const std::string& node_name, std::shared_ptr<NodeBase> node_ptr) {
    if (node_map_.find(node_name) != node_map_.end()) {
      throw std::runtime_error(std::format("[NodeMap] Node '{}' already exists!", node_name));
    }
    node_map_[node_name] = node_ptr;
  }

  /**
   * @brief Retrieves a node pointer by its name.
   *
   * This method searches for a node with the specified name and returns a reference to its
   * `std::shared_ptr<NodeBase>`. If the node does not exist, the method throws a `std::runtime_error`.
   *
   * @param key The name identifier of the node to retrieve.
   * @return A constant reference to the `std::shared_ptr<NodeBase>` corresponding to the node.
   *
   * @throws std::runtime_error If no node with the specified name is found in the map.
   *
   * @example
   * @code
   * auto node = node_map.GetNodePtr("Node1");
   * node->Execute();
   * @endcode
   */
  const std::shared_ptr<NodeBase>& GetNodePtr(const std::string& key) const {
    auto it = node_map_.find(key);
    if (it == node_map_.end()) {
      throw std::runtime_error(std::format("[NodeMap] Node '{}' not found!", key));
    }
    return it->second;
  }

 private:
  /**
   * @brief Internal map storing node names and their corresponding pointers.
   *
   * The key is a unique string representing the node's name, and the value is a `std::shared_ptr`
   * to the `NodeBase` instance. This map ensures efficient retrieval and management of nodes.
   */
  std::unordered_map<std::string, std::shared_ptr<NodeBase>> node_map_;
};

}  // namespace openrobot::ocm
