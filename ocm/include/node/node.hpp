#pragma once
#include <atomic>
#include <cstdint>

namespace openrobot::ocm {
enum NodeState : uint8_t { INIT = 0, RUNNING, STANDBY };
class NodeBase {
 public:
  NodeBase() { state_.store(NodeState::INIT); }
  NodeBase& operator=(const NodeBase&) = delete;
  NodeBase(NodeBase&&) = delete;
  virtual ~NodeBase() = default;

  virtual void Init() = 0;
  virtual void Run() = 0;
  virtual void Output() = 0;
  virtual bool TryEnter() = 0;
  virtual bool TryExit() = 0;
  NodeState GetState() const { return state_.load(); }

 private:
  void SetState(NodeState state) { state_.store(state); }
  std::atomic<NodeState> state_;
};
}  // namespace openrobot::ocm
