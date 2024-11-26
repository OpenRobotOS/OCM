
#pragma once

#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include "node/node.hpp"

namespace openrobot::ocm {

/**
 * @class NodeMap
 * @brief 管理一组节点，允许通过名称添加和检索节点。Manages a collection of nodes, allowing addition and retrieval of nodes by name.
 *
 * NodeMap类提供了存储和管理通过唯一字符串标识的节点的功能。It provides functionality to store and manage nodes identified by unique string names.
 * 它确保每个节点名称在映射中是唯一的，并提供添加新节点和检索现有节点的方法。It ensures that each node name is unique within the map and provides
 * methods to add new nodes and retrieve existing nodes. 尝试添加具有重复名称的节点或检索不存在的节点将导致异常。Attempting to add a node with a
 * duplicate name or retrieving a non-existent node will result in an exception。
 *
 * @note 此类被标记为 `final` 以防止继承。@note This class is marked as `final` to prevent inheritance.
 */
class NodeMap final {
 public:
  /**
   * @brief 默认构造函数。Default constructor.
   *
   * 初始化一个空的 `NodeMap`。Initializes an empty `NodeMap`.
   */
  NodeMap() = default;

  /**
   * @brief 默认析构函数。Default destructor.
   *
   * 清理 `NodeMap`。由于所有管理的节点都通过 `std::shared_ptr` 处理，Explicit cleanup is not required.
   *
   * Cleans up the `NodeMap`. As all managed nodes are handled via `std::shared_ptr`, explicit cleanup is not required.
   */
  ~NodeMap() = default;

  /**
   * @brief 向映射中添加一个新节点。Adds a new node to the map.
   *
   * 此方法将一个具有指定名称和指针的新节点插入到 `NodeMap` 中。This method inserts a new node into the `NodeMap` with the specified name and pointer.
   * 如果给定名称的节点已经存在，方法将抛出 `std::runtime_error`。If a node with the given name already exists, the method throws a
   * `std::runtime_error`.
   *
   * @param node_name 节点的唯一名称标识。The unique name identifier for the node.
   * @param node_ptr 一个指向要添加的节点的 `std::shared_ptr`。A `std::shared_ptr` to the node to be added.
   *
   * @throws std::runtime_error 如果映射中已存在具有给定名称的节点。If a node with the given name already exists in the map.
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
   * @brief 通过名称检索节点指针。Retrieves a node pointer by its name.
   *
   * 此方法搜索具有指定名称的节点，并返回其 `std::shared_ptr<NodeBase>` 的引用。This method searches for a node with the specified name and returns a
   * reference to its `std::shared_ptr<NodeBase>`. 如果节点不存在，方法将抛出 `std::runtime_error`。If the node does not exist, the method throws a
   * `std::runtime_error`.
   *
   * @param key 要检索的节点的名称标识。The name identifier of the node to retrieve.
   * @return 节点对应的 `std::shared_ptr<NodeBase>` 的常量引用。A constant reference to the `std::shared_ptr<NodeBase>` corresponding to the node.
   *
   * @throws std::runtime_error 如果在映射中未找到具有指定名称的节点。If no node with the specified name is found in the map.
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
   * @brief 存储节点名称及其对应指针的内部映射。Internal map storing node names and their corresponding pointers.
   *
   * 键是表示节点名称的唯一字符串，值是指向 `NodeBase` 实例的 `std::shared_ptr`。The key is a unique string representing the node's name, and the
   * value is a `std::shared_ptr` 指向 `NodeBase` 实例。这张映射确保了节点的高效检索和管理。to the `NodeBase` instance. This map ensures efficient
   * retrieval and management of nodes.
   */
  std::unordered_map<std::string, std::shared_ptr<NodeBase>> node_map_;
};

}  // namespace openrobot::ocm
