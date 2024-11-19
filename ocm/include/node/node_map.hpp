#pragma once
#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include "node/node.hpp"

namespace openrobot::ocm {

class NodeMap final {
 public:
  NodeMap() = default;
  ~NodeMap() = default;

  void AddNode(const std::string& node_name, std::shared_ptr<NodeBase> node_ptr) {
    if (node_map_.find(node_name) != node_map_.end()) {
      throw std::runtime_error(std::format("[NodeMap] node {} already exists!", node_name));
    }
    node_map_[node_name] = node_ptr;
  }
  const std::shared_ptr<NodeBase>& GetNodePtr(const std::string& key) {
    if (node_map_.find(key) == node_map_.end()) {
      throw std::runtime_error(std::format("[NodeMap] node {} not found!", key));
    }
    return node_map_.at(key);
  }

 private:
  std::unordered_map<std::string, std::shared_ptr<NodeBase>> node_map_;
};
}  // namespace openrobot::ocm
