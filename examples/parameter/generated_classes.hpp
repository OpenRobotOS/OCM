#pragma once
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

namespace openrobot::ocm {
namespace auto_ActionSetting {
namespace auto_B {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_B
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_B {
class Module {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionSetting::auto_B::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionSetting::auto_B::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionSetting::auto_B::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_B
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_B {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionSetting::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionSetting::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionSetting::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_B {
class ActionSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionSetting::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionSetting::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionSetting::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_B {
class auto_B {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) {
      resident_setting_.clear();
      for (auto& item : node["RESIDENT_SETTING"]) {
        auto_ActionSetting::auto_B::ResidentSetting elem;
        elem.update_from_yaml(item);
        resident_setting_.push_back(elem);
      }
    }
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionSetting::auto_B::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const std::vector<auto_ActionSetting::auto_B::ResidentSetting>& ResidentSetting() const { return resident_setting_; }

  const std::vector<auto_ActionSetting::auto_B::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  std::vector<auto_ActionSetting::auto_B::ResidentSetting> resident_setting_;
  std::vector<auto_ActionSetting::auto_B::ActionSetting> action_setting_;
};

}  // namespace auto_B
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_A {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_A
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_A {
class Module {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionSetting::auto_A::Runner elem;
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionSetting::auto_A::Runner>& Runner() const { return runner_; }

  const std::vector<double>& Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionSetting::auto_A::Runner> runner_;
  std::vector<double> period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_A
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_A {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionSetting::auto_A::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionSetting::auto_A::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionSetting::auto_A::Module> module_;
};

}  // namespace auto_A
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_A {
class ActionSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionSetting::auto_A::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionSetting::auto_A::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionSetting::auto_A::Module> module_;
};

}  // namespace auto_A
}  // namespace auto_ActionSetting

namespace auto_ActionSetting {
namespace auto_A {
class auto_A {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) {
      resident_setting_.clear();
      for (auto& item : node["RESIDENT_SETTING"]) {
        auto_ActionSetting::auto_A::ResidentSetting elem;
        elem.update_from_yaml(item);
        resident_setting_.push_back(elem);
      }
    }
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionSetting::auto_A::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const std::vector<auto_ActionSetting::auto_A::ResidentSetting>& ResidentSetting() const { return resident_setting_; }

  const std::vector<auto_ActionSetting::auto_A::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  std::vector<auto_ActionSetting::auto_A::ResidentSetting> resident_setting_;
  std::vector<auto_ActionSetting::auto_A::ActionSetting> action_setting_;
};

}  // namespace auto_A
}  // namespace auto_ActionSetting

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_B {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_B
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_B {
class Module {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_B::Runner elem;
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_B::Runner>& Runner() const { return runner_; }

  const std::vector<double>& Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_B::Runner> runner_;
  std::vector<double> period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_B
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_B {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test1::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionManager::auto_Test1::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test1::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_B {
class ActionSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test1::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionManager::auto_Test1::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test1::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_B {
class auto_B {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) {
      resident_setting_.clear();
      for (auto& item : node["RESIDENT_SETTING"]) {
        auto_ActionManager::auto_Test1::auto_B::ResidentSetting elem;
        elem.update_from_yaml(item);
        resident_setting_.push_back(elem);
      }
    }
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionManager::auto_Test1::auto_B::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_B::ResidentSetting>& ResidentSetting() const { return resident_setting_; }

  const std::vector<auto_ActionManager::auto_Test1::auto_B::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_B::ResidentSetting> resident_setting_;
  std::vector<auto_ActionManager::auto_Test1::auto_B::ActionSetting> action_setting_;
};

}  // namespace auto_B
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Gamepad {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class SecurityGuard {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class BTree {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Monitor {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Ros2Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class HybridSub {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class LandmarkSub {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Fusion {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class HybridPub {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<double>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class TaskTrajectoryMove {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["gamepad"]) gamepad_.update_from_yaml(node["gamepad"]);
    if (node["security_guard"]) security_guard_.update_from_yaml(node["security_guard"]);
    if (node["b_tree"]) b_tree_.update_from_yaml(node["b_tree"]);
    if (node["monitor"]) monitor_.update_from_yaml(node["monitor"]);
    if (node["ros2_runner"]) ros2_runner_.update_from_yaml(node["ros2_runner"]);
    if (node["hybrid_sub"]) hybrid_sub_.update_from_yaml(node["hybrid_sub"]);
    if (node["landmark_sub"]) landmark_sub_.update_from_yaml(node["landmark_sub"]);
    if (node["fusion"]) fusion_.update_from_yaml(node["fusion"]);
    if (node["hybrid_pub"]) hybrid_pub_.update_from_yaml(node["hybrid_pub"]);
    if (node["task_trajectory_move"]) task_trajectory_move_.update_from_yaml(node["task_trajectory_move"]);
  }

  const auto_ActionManager::auto_Test1::auto_A::Gamepad& Gamepad() const { return gamepad_; }

  const auto_ActionManager::auto_Test1::auto_A::SecurityGuard& SecurityGuard() const { return security_guard_; }

  const auto_ActionManager::auto_Test1::auto_A::BTree& BTree() const { return b_tree_; }

  const auto_ActionManager::auto_Test1::auto_A::Monitor& Monitor() const { return monitor_; }

  const auto_ActionManager::auto_Test1::auto_A::Ros2Runner& Ros2Runner() const { return ros2_runner_; }

  const auto_ActionManager::auto_Test1::auto_A::HybridSub& HybridSub() const { return hybrid_sub_; }

  const auto_ActionManager::auto_Test1::auto_A::LandmarkSub& LandmarkSub() const { return landmark_sub_; }

  const auto_ActionManager::auto_Test1::auto_A::Fusion& Fusion() const { return fusion_; }

  const auto_ActionManager::auto_Test1::auto_A::HybridPub& HybridPub() const { return hybrid_pub_; }

  const auto_ActionManager::auto_Test1::auto_A::TaskTrajectoryMove& TaskTrajectoryMove() const { return task_trajectory_move_; }

 private:
  auto_ActionManager::auto_Test1::auto_A::Gamepad gamepad_;
  auto_ActionManager::auto_Test1::auto_A::SecurityGuard security_guard_;
  auto_ActionManager::auto_Test1::auto_A::BTree b_tree_;
  auto_ActionManager::auto_Test1::auto_A::Monitor monitor_;
  auto_ActionManager::auto_Test1::auto_A::Ros2Runner ros2_runner_;
  auto_ActionManager::auto_Test1::auto_A::HybridSub hybrid_sub_;
  auto_ActionManager::auto_Test1::auto_A::LandmarkSub landmark_sub_;
  auto_ActionManager::auto_Test1::auto_A::Fusion fusion_;
  auto_ActionManager::auto_Test1::auto_A::HybridPub hybrid_pub_;
  auto_ActionManager::auto_Test1::auto_A::TaskTrajectoryMove task_trajectory_move_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class Module {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test1::auto_A::Runner elem;
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Runner>& Runner() const { return runner_; }

  const std::vector<double>& Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionManager::auto_Test1::auto_A::Runner> runner_;
  std::vector<double> period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class ActionSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test1::auto_A::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test1::auto_A::Module> module_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test1 {
namespace auto_A {
class auto_A {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) resident_setting_.update_from_yaml(node["RESIDENT_SETTING"]);
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionManager::auto_Test1::auto_A::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const auto_ActionManager::auto_Test1::auto_A::ResidentSetting& ResidentSetting() const { return resident_setting_; }

  const std::vector<auto_ActionManager::auto_Test1::auto_A::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  auto_ActionManager::auto_Test1::auto_A::ResidentSetting resident_setting_;
  std::vector<auto_ActionManager::auto_Test1::auto_A::ActionSetting> action_setting_;
};

}  // namespace auto_A
}  // namespace auto_Test1
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_B {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

 private:
  std::string name_;
  bool enable_;
  bool debug_info_;
};

}  // namespace auto_B
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_B {
class Module {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_B::Runner elem;
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_B::Runner>& Runner() const { return runner_; }

  const std::vector<double>& Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_B::Runner> runner_;
  std::vector<double> period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_B
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_B {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test2::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionManager::auto_Test2::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test2::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_B {
class ActionSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["module"]) {
      module_.clear();
      for (auto& item : node["module"]) {
        auto_ActionManager::auto_Test2::auto_B::Module elem;
        elem.update_from_yaml(item);
        module_.push_back(elem);
      }
    }
  }

  std::string Name() const { return name_; }

  const std::vector<auto_ActionManager::auto_Test2::auto_B::Module>& Module() const { return module_; }

 private:
  std::string name_;
  std::vector<auto_ActionManager::auto_Test2::auto_B::Module> module_;
};

}  // namespace auto_B
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_B {
class auto_B {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["RESIDENT_SETTING"]) {
      resident_setting_.clear();
      for (auto& item : node["RESIDENT_SETTING"]) {
        auto_ActionManager::auto_Test2::auto_B::ResidentSetting elem;
        elem.update_from_yaml(item);
        resident_setting_.push_back(elem);
      }
    }
    if (node["ACTION_SETTING"]) {
      action_setting_.clear();
      for (auto& item : node["ACTION_SETTING"]) {
        auto_ActionManager::auto_Test2::auto_B::ActionSetting elem;
        elem.update_from_yaml(item);
        action_setting_.push_back(elem);
      }
    }
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_B::ResidentSetting>& ResidentSetting() const { return resident_setting_; }

  const std::vector<auto_ActionManager::auto_Test2::auto_B::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_B::ResidentSetting> resident_setting_;
  std::vector<auto_ActionManager::auto_Test2::auto_B::ActionSetting> action_setting_;
};

}  // namespace auto_B
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["name"]) name_ = node["name"].as<std::string>();
    if (node["enable"]) enable_ = node["enable"].as<bool>();
    if (node["debug_info"]) debug_info_ = node["debug_info"].as<bool>();
  }

  std::string Name() const { return name_; }

  bool Enable() const { return enable_; }

  bool DebugInfo() const { return debug_info_; }

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
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class SecurityGuard {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class BTree {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Monitor {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Ros2Runner {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class HybridSub {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class LandmarkSub {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Fusion {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class HybridPub {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  double Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  double period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class TaskTrajectoryMove {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["runner"]) {
      runner_.clear();
      for (auto& item : node["runner"]) {
        auto_ActionManager::auto_Test2::auto_A::Runner elem;
        elem.update_from_yaml(item);
        runner_.push_back(elem);
      }
    }
    if (node["period"]) period_ = node["period"].as<int>();
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
    if (node["start_delay"]) start_delay_ = node["start_delay"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  int Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

  int StartDelay() const { return start_delay_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  int period_;
  int priority_;
  int cpu_affinity_;
  int start_delay_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class ResidentSetting {
 public:
  void update_from_yaml(const YAML::Node& node) {
    if (node["gamepad"]) gamepad_.update_from_yaml(node["gamepad"]);
    if (node["security_guard"]) security_guard_.update_from_yaml(node["security_guard"]);
    if (node["b_tree"]) b_tree_.update_from_yaml(node["b_tree"]);
    if (node["monitor"]) monitor_.update_from_yaml(node["monitor"]);
    if (node["ros2_runner"]) ros2_runner_.update_from_yaml(node["ros2_runner"]);
    if (node["hybrid_sub"]) hybrid_sub_.update_from_yaml(node["hybrid_sub"]);
    if (node["landmark_sub"]) landmark_sub_.update_from_yaml(node["landmark_sub"]);
    if (node["fusion"]) fusion_.update_from_yaml(node["fusion"]);
    if (node["hybrid_pub"]) hybrid_pub_.update_from_yaml(node["hybrid_pub"]);
    if (node["task_trajectory_move"]) task_trajectory_move_.update_from_yaml(node["task_trajectory_move"]);
  }

  const auto_ActionManager::auto_Test2::auto_A::Gamepad& Gamepad() const { return gamepad_; }

  const auto_ActionManager::auto_Test2::auto_A::SecurityGuard& SecurityGuard() const { return security_guard_; }

  const auto_ActionManager::auto_Test2::auto_A::BTree& BTree() const { return b_tree_; }

  const auto_ActionManager::auto_Test2::auto_A::Monitor& Monitor() const { return monitor_; }

  const auto_ActionManager::auto_Test2::auto_A::Ros2Runner& Ros2Runner() const { return ros2_runner_; }

  const auto_ActionManager::auto_Test2::auto_A::HybridSub& HybridSub() const { return hybrid_sub_; }

  const auto_ActionManager::auto_Test2::auto_A::LandmarkSub& LandmarkSub() const { return landmark_sub_; }

  const auto_ActionManager::auto_Test2::auto_A::Fusion& Fusion() const { return fusion_; }

  const auto_ActionManager::auto_Test2::auto_A::HybridPub& HybridPub() const { return hybrid_pub_; }

  const auto_ActionManager::auto_Test2::auto_A::TaskTrajectoryMove& TaskTrajectoryMove() const { return task_trajectory_move_; }

 private:
  auto_ActionManager::auto_Test2::auto_A::Gamepad gamepad_;
  auto_ActionManager::auto_Test2::auto_A::SecurityGuard security_guard_;
  auto_ActionManager::auto_Test2::auto_A::BTree b_tree_;
  auto_ActionManager::auto_Test2::auto_A::Monitor monitor_;
  auto_ActionManager::auto_Test2::auto_A::Ros2Runner ros2_runner_;
  auto_ActionManager::auto_Test2::auto_A::HybridSub hybrid_sub_;
  auto_ActionManager::auto_Test2::auto_A::LandmarkSub landmark_sub_;
  auto_ActionManager::auto_Test2::auto_A::Fusion fusion_;
  auto_ActionManager::auto_Test2::auto_A::HybridPub hybrid_pub_;
  auto_ActionManager::auto_Test2::auto_A::TaskTrajectoryMove task_trajectory_move_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class Module {
 public:
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
    if (node["priority"]) priority_ = node["priority"].as<int>();
    if (node["cpu_affinity"]) cpu_affinity_ = node["cpu_affinity"].as<int>();
  }

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Runner>& Runner() const { return runner_; }

  const std::vector<double>& Period() const { return period_; }

  int Priority() const { return priority_; }

  int CpuAffinity() const { return cpu_affinity_; }

 private:
  std::vector<auto_ActionManager::auto_Test2::auto_A::Runner> runner_;
  std::vector<double> period_;
  int priority_;
  int cpu_affinity_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

namespace auto_ActionManager {
namespace auto_Test2 {
namespace auto_A {
class ActionSetting {
 public:
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

  const std::vector<auto_ActionManager::auto_Test2::auto_A::Module>& Module() const { return module_; }

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

  const std::vector<auto_ActionManager::auto_Test2::auto_A::ActionSetting>& ActionSetting() const { return action_setting_; }

 private:
  auto_ActionManager::auto_Test2::auto_A::ResidentSetting resident_setting_;
  std::vector<auto_ActionManager::auto_Test2::auto_A::ActionSetting> action_setting_;
};

}  // namespace auto_A
}  // namespace auto_Test2
}  // namespace auto_ActionManager

class ConfigCollect {
 public:
  void update_from_yaml(const std::string& base_path) {
    YAML::Node node;

    // 更新 auto_ActionSetting::auto_B::auto_B
    node = YAML::LoadFile(base_path + "/action_setting/b.yaml");
    action_setting_b_.update_from_yaml(node);

    // 更新 auto_ActionSetting::auto_A::auto_A
    node = YAML::LoadFile(base_path + "/action_setting/a.yaml");
    action_setting_a_.update_from_yaml(node);

    // 更新 auto_ActionManager::auto_Test1::auto_B::auto_B
    node = YAML::LoadFile(base_path + "/action_manager/test1/b.yaml");
    action_manager_test1_b_.update_from_yaml(node);

    // 更新 auto_ActionManager::auto_Test1::auto_A::auto_A
    node = YAML::LoadFile(base_path + "/action_manager/test1/a.yaml");
    action_manager_test1_a_.update_from_yaml(node);

    // 更新 auto_ActionManager::auto_Test2::auto_B::auto_B
    node = YAML::LoadFile(base_path + "/action_manager/test2/b.yaml");
    action_manager_test2_b_.update_from_yaml(node);

    // 更新 auto_ActionManager::auto_Test2::auto_A::auto_A
    node = YAML::LoadFile(base_path + "/action_manager/test2/a.yaml");
    action_manager_test2_a_.update_from_yaml(node);
  }

  const auto_ActionSetting::auto_B::auto_B& get_action_setting_b_() const { return action_setting_b_; }

  const auto_ActionSetting::auto_A::auto_A& get_action_setting_a_() const { return action_setting_a_; }

  const auto_ActionManager::auto_Test1::auto_B::auto_B& get_action_manager_test1_b_() const { return action_manager_test1_b_; }

  const auto_ActionManager::auto_Test1::auto_A::auto_A& get_action_manager_test1_a_() const { return action_manager_test1_a_; }

  const auto_ActionManager::auto_Test2::auto_B::auto_B& get_action_manager_test2_b_() const { return action_manager_test2_b_; }

  const auto_ActionManager::auto_Test2::auto_A::auto_A& get_action_manager_test2_a_() const { return action_manager_test2_a_; }

 private:
  auto_ActionSetting::auto_B::auto_B action_setting_b_;
  auto_ActionSetting::auto_A::auto_A action_setting_a_;
  auto_ActionManager::auto_Test1::auto_B::auto_B action_manager_test1_b_;
  auto_ActionManager::auto_Test1::auto_A::auto_A action_manager_test1_a_;
  auto_ActionManager::auto_Test2::auto_B::auto_B action_manager_test2_b_;
  auto_ActionManager::auto_Test2::auto_A::auto_A action_manager_test2_a_;
};

}  // namespace openrobot::ocm
