#pragma once
#include <atomic>
#include "common/enum.hpp"

namespace openrobot::ocm {

class NodeBase {
 public:
  NodeBase() { state_.store(NodeState::INIT); }
  NodeBase& operator=(const NodeBase&) = delete;
  NodeBase(NodeBase&&) = delete;
  virtual ~NodeBase() = default;

  virtual void Init() = 0;
  virtual void Execute() = 0;
  virtual void Output() = 0;
  virtual bool TryEnter() = 0;
  virtual bool TryExit() = 0;

  void RunOnce() {
    Execute();
    SetState(NodeState::RUNNING);
  }
  bool EnterCheck() { return TryEnter(); }
  bool ExitCheck() {
    SetState(NodeState::STANDBY);
    return TryExit();
  }
  NodeState GetState() const { return state_.load(); }

 protected:
  void SetState(NodeState state) { state_.store(state); }
  std::atomic<NodeState> state_;
};
}  // namespace openrobot::ocm
