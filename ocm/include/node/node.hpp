#pragma once

namespace openrobot::ocm {
class NodeBase {
 public:
  NodeBase() = default;
  NodeBase& operator=(const NodeBase&) = delete;
  NodeBase(NodeBase&&) = delete;
  virtual ~NodeBase() { Shutdown(); }

  virtual void Init();
  virtual void Run();
  virtual void Shutdown();
  virtual void Output();
  virtual bool EnterCheck();
  virtual bool ExitCheck();
  virtual void DebugInfoPrint();
};
}  // namespace openrobot::ocm
