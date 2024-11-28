#include "node/node.hpp"

namespace openrobot::ocm {

NodeBase::NodeBase(const std::string& node_name) : node_name_(node_name) {
  state_.store(NodeState::INIT);  // 初始化节点状态为INIT
}
void NodeBase::SetState(NodeState state) {
  state_.store(state);  // 设置节点状态
}
NodeState NodeBase::GetState() const {
  return state_.load();  // 获取当前节点状态
}
const std::string& NodeBase::GetNodeName() const {
  return node_name_;  // 返回节点名称
}
}  // namespace openrobot::ocm
