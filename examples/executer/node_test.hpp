#include <iostream>
#include "node/node.hpp"

class NodeA : public openrobot::ocm::NodeBase {
 public:
  NodeA() : openrobot::ocm::NodeBase() {}
  void Init() override { std::cout << "NodeA Init" << std::endl; }
  void Run() override { std::cout << "NodeA Run" << std::endl; }
  void Output() override { std::cout << "NodeA Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeA TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeA TryExit" << std::endl;
    return true;
  }
};

class NodeB : public openrobot::ocm::NodeBase {
 public:
  NodeB() : openrobot::ocm::NodeBase() {}
  void Init() override { std::cout << "NodeB Init" << std::endl; }
  void Run() override { std::cout << "NodeB Run" << std::endl; }
  void Output() override { std::cout << "NodeB Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeB TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeB TryExit" << std::endl;
    return true;
  }
};

class NodeC : public openrobot::ocm::NodeBase {
 public:
  NodeC() : openrobot::ocm::NodeBase() {}
  void Init() override { std::cout << "NodeC Init" << std::endl; }
  void Run() override { std::cout << "NodeC Run" << std::endl; }
  void Output() override { std::cout << "NodeC Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeC TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeC TryExit" << std::endl;
    return true;
  }
};

class NodeD : public openrobot::ocm::NodeBase {
 public:
  NodeD() : openrobot::ocm::NodeBase() {}
  void Init() override { std::cout << "NodeD Init" << std::endl; }
  void Run() override { std::cout << "NodeD Run" << std::endl; }
  void Output() override { std::cout << "NodeD Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeD TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeD TryExit" << std::endl;
    return true;
  }
};

class NodeE : public openrobot::ocm::NodeBase {
 public:
  NodeE() : openrobot::ocm::NodeBase() {}
  void Init() override { std::cout << "NodeE Init" << std::endl; }
  void Run() override { std::cout << "NodeE Run" << std::endl; }
  void Output() override { std::cout << "NodeE Output" << std::endl; }
  bool TryEnter() override {
    std::cout << "NodeE TryEnter" << std::endl;
    return true;
  }
  bool TryExit() override {
    std::cout << "NodeE TryExit" << std::endl;
    return true;
  }
};