#include <iostream>
#include "node/node.hpp"
#include "node/node_map.hpp"

class TestNode : public openrobot::ocm::NodeBase {
 public:
  void SetA(std::shared_ptr<int> a) { a_ = a; }
  void Init() override { std::cout << "[TestNode] Init" << *a_ << std::endl; }
  void Run() override { std::cout << "[TestNode] Run" << *a_ << std::endl; }
  void Shutdown() override { std::cout << "[TestNode] Shutdown" << *a_ << std::endl; }
  void Output() override { std::cout << "[TestNode] Output" << *a_ << std::endl; }
  bool EnterCheck() override {
    std::cout << "[TestNode] EnterCheck" << *a_ << std::endl;
    return true;
  }
  bool ExitCheck() override {
    std::cout << "[TestNode] ExitCheck" << *a_ << std::endl;
    return true;
  }
  void DebugInfoPrint() override { std::cout << "[TestNode] DebugInfoPrint" << *a_ << std::endl; }

 private:
  std::shared_ptr<int> a_;
};

int main() {
  auto node_map = openrobot::ocm::NodeMap();
  auto test_node = std::make_shared<TestNode>();
  test_node->SetA(std::make_shared<int>(10));
  node_map.AddNode("test_node", test_node);
  auto node_ptr = node_map.GetNodePtr("test_node");
  node_ptr->Init();
  node_ptr->Run();
  node_ptr->Shutdown();
  return 0;
}
