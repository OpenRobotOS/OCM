#pragma once
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

namespace openrobot::ocm {
namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Runner {
 public:
  Runner() = default;
  ~Runner() = default;

  Runner(const Runner&) = default;
  Runner& operator=(const Runner&) = default;

  Runner(Runner&&) = default;
  Runner& operator=(Runner&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "Runner:" << std::endl;
    std::cout << indent << "    name_: " << name_ << std::endl;
    std::cout << indent << "    enable_: " << enable_ << std::endl;
    std::cout << indent << "    debug_info_: " << debug_info_ << std::endl;
  }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Gamepad {
 public:
  Gamepad() = default;
  ~Gamepad() = default;

  Gamepad(const Gamepad&) = default;
  Gamepad& operator=(const Gamepad&) = default;

  Gamepad(Gamepad&&) = default;
  Gamepad& operator=(Gamepad&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<double>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<double>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<double>();
  }

  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> Runner() const { return runner_; }

  double Period() const { return period_; }

  double Priority() const { return priority_; }

  double CpuAffinity() const { return cpu_affinity_; }

  double StartDelay() const { return start_delay_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "Gamepad:" << std::endl;
    std::cout << indent << "    runner_: [" << std::endl;
    for (const auto& item : runner_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    period_: " << period_ << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: " << cpu_affinity_ << std::endl;
    std::cout << indent << "    start_delay_: " << start_delay_ << std::endl;
  }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  double priority_;
  double cpu_affinity_;
  double start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class ResidentSetting {
 public:
  ResidentSetting() = default;
  ~ResidentSetting() = default;

  ResidentSetting(const ResidentSetting&) = default;
  ResidentSetting& operator=(const ResidentSetting&) = default;

  ResidentSetting(ResidentSetting&&) = default;
  ResidentSetting& operator=(ResidentSetting&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["gamepad"]) gamepad_.update_from_yaml(node["gamepad"]);
  }

  const auto_ActionManager::auto_Test2::auto_A::Gamepad& Gamepad() const { return gamepad_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ResidentSetting:" << std::endl;
    std::cout << indent << "    gamepad_:" << std::endl;
    gamepad_.print(indent_level + 1);
  }

 private:
  auto_ActionManager::auto_Test2::auto_A::Gamepad gamepad_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Module {
 public:
  Module() = default;
  ~Module() = default;

  Module(const Module&) = default;
  Module& operator=(const Module&) = default;

  Module(Module&&) = default;
  Module& operator=(Module&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) {
      period_.clear();
      for (auto& item : node["period"]) {
        period_.push_back(item.as<double>());
      }
    }
    if (node["priority"]) priority_ = node["priority"].as<double>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<double>();
  }

  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> Runner() const { return runner_; }

  std::vector<double> Period() const { return period_; }

  double Priority() const { return priority_; }

  double CpuAffinity() const { return cpu_affinity_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "Module:" << std::endl;
    std::cout << indent << "    runner_: [" << std::endl;
    for (const auto& item : runner_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    period_: [" << std::endl;
    for (const auto& item : period_) {
      std::cout << indent << "        " << item << std::endl;
    }
    std::cout << indent << "    ]" << std::endl;
    std::cout << indent << "    priority_: " << priority_ << std::endl;
    std::cout << indent << "    cpu_affinity_: " << cpu_affinity_ << std::endl;
  }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  std::vector<double> period_;
  double priority_;
  double cpu_affinity_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class ActionSetting {
 public:
  ActionSetting() = default;
  ~ActionSetting() = default;

  ActionSetting(const ActionSetting&) = default;
  ActionSetting& operator=(const ActionSetting&) = default;

  ActionSetting(ActionSetting&&) = default;
  ActionSetting& operator=(ActionSetting&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test2::auto_A::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  std::vector<auto_ActionManager::auto_Test2::auto_A::Module> Module() const { return module_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ActionSetting:" << std::endl;
    std::cout << indent << "    name_: " << name_ << std::endl;
    std::cout << indent << "    module_: [" << std::endl;
    for (const auto& item : module_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test2::auto_A::Module> module_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class auto_A {
 public:
  auto_A() = default;
  ~auto_A() = default;

  auto_A(const auto_A&) = default;
  auto_A& operator=(const auto_A&) = default;

  auto_A(auto_A&&) = default;
  auto_A& operator=(auto_A&&) = default;

  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) resident_setting_.update_from_yaml(node["RESIDENT_SETTING"]);
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionManager::auto_Test2::auto_A::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const auto_ActionManager::auto_Test2::auto_A::ResidentSetting& ResidentSetting() const { return resident_setting_; }

  std::vector<auto_ActionManager::auto_Test2::auto_A::ActionSetting> ActionSetting() const { return action_setting_; }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "auto_A:" << std::endl;
    std::cout << indent << "    resident_setting_:" << std::endl;
    resident_setting_.print(indent_level + 1);
    std::cout << indent << "    action_setting_: [" << std::endl;
    for (const auto& item : action_setting_) {
      item.print(indent_level + 2);
    }
    std::cout << indent << "    ]" << std::endl;
  }

 private:
  auto_ActionManager::auto_Test2::auto_A::ResidentSetting resident_setting_;
  std::vector<auto_ActionManager::auto_Test2::auto_A::ActionSetting> action_setting_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

class ConfigCollect {
 public:
  ConfigCollect() = default;
  ~ConfigCollect() = default;

  ConfigCollect(const ConfigCollect&) = delete;
  ConfigCollect& operator=(const ConfigCollect&) = delete;

  ConfigCollect(ConfigCollect&&) = delete;
  ConfigCollect& operator=(ConfigCollect&&) = delete;

  void update_from_yaml(const std::string& name, const std::string& base_path) {
    if (name.empty()) {
      return;
    }
    bool matched = false;
    if (name == "action_manager_test2_a") {
      update_from_yaml_action_manager_test2_a(base_path);
      matched = true;
    }
    if (!matched) {
      // 未找到匹配的配置项
      std::cerr << "[ConfigCollect] No matching configuration for '" << name << "'" << std::endl;
    }
  }

  void update_from_yaml_all(const std::string& base_path) { update_from_yaml_action_manager_test2_a(base_path); }

  void print(int indent_level = 0) const {
    std::string indent(indent_level * 4, ' ');
    std::cout << indent << "ActionManager_Test2_A:" << std::endl;
    {
      std::shared_lock<std::shared_mutex> lock(m_ActionManager_Test2_A);
      ActionManager_Test2_A.print(indent_level + 1);
    }
  }

  auto_ActionManager::auto_Test2::auto_A::auto_A get_ActionManager_Test2_A() const {
    std::shared_lock<std::shared_mutex> lock(m_ActionManager_Test2_A);
    return ActionManager_Test2_A;
  }

 private:
  auto_ActionManager::auto_Test2::auto_A::auto_A ActionManager_Test2_A;
  mutable std::shared_mutex m_ActionManager_Test2_A;

  void update_from_yaml_action_manager_test2_a(const std::string& base_path) {
    YAML::Node node;
    // 更新 auto_ActionManager::auto_Test2::auto_A::auto_A
    node = YAML::LoadFile(base_path + "/action_manager/test2/a.yaml");
    {
      std::unique_lock<std::shared_mutex> lock(m_ActionManager_Test2_A);
      ActionManager_Test2_A.update_from_yaml(node);
    }
  }
};

}  // namespace openrobot::ocm
