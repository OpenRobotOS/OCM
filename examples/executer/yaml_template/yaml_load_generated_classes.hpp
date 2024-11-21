/*
 * Automatically generate files, manual modification is strictly prohibited!
 */
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <shared_mutex>
#include <mutex>

namespace openrobot::ocm {
namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_Executer {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_Executer {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["executer_cpu_affinity"]) {
            executer_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["executer_cpu_affinity"]) {
                executer_cpu_affinity_.push_back(item.as<double>());
            }
        }
        if(auto_yaml_node["idle_task_cpu_affinity"]) {
            idle_task_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["idle_task_cpu_affinity"]) {
                idle_task_cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> ExecuterCpuAffinity() const {
            return executer_cpu_affinity_;
        }

        std::vector<double> IdleTaskCpuAffinity() const {
            return idle_task_cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    executer_cpu_affinity_: [" << std::endl;
        for(const auto& item : executer_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    idle_task_cpu_affinity_: [" << std::endl;
        for(const auto& item : idle_task_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> executer_cpu_affinity_;
        std::vector<double> idle_task_cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_Executer {
class Executer {
public:
    Executer() = default;
    ~Executer() = default;

    Executer(const Executer&) = default;
    Executer& operator=(const Executer&) = default;

    Executer(Executer&&) = default;
    Executer& operator=(Executer&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["package_name"]) package_name_ = auto_yaml_node["package_name"].as<std::string>();
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    }

        std::string PackageName() const {
            return package_name_;
        }

        const auto_Task::auto_TaskConfig::auto_Executer::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task::auto_TaskConfig::auto_Executer::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "Executer:" << std::endl;
        std::cout << indent << "    package_name_: " << package_name_ << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
    }

private:
        std::string package_name_;
        auto_Task::auto_TaskConfig::auto_Executer::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task::auto_TaskConfig::auto_Executer::auto_SystemSetting::SystemSetting system_setting_;
};

} // namespace auto_Executer
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_NodeList {
class NodeList {
public:
    NodeList() = default;
    ~NodeList() = default;

    NodeList(const NodeList&) = default;
    NodeList& operator=(const NodeList&) = default;

    NodeList(NodeList&&) = default;
    NodeList& operator=(NodeList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
        if(auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
    }

        std::string NodeName() const {
            return node_name_;
        }

        bool OutputEnable() const {
            return output_enable_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "NodeList:" << std::endl;
        std::cout << indent << "    node_name_: " << node_name_ << std::endl;
        std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
    }

private:
        std::string node_name_;
        bool output_enable_;
};

} // namespace auto_NodeList
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["cpu_affinity"]) {
            cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["cpu_affinity"]) {
                cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> CpuAffinity() const {
            return cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    cpu_affinity_: [" << std::endl;
        for(const auto& item : cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_LaunchSetting {
class LaunchSetting {
public:
    LaunchSetting() = default;
    ~LaunchSetting() = default;

    LaunchSetting(const LaunchSetting&) = default;
    LaunchSetting& operator=(const LaunchSetting&) = default;

    LaunchSetting(LaunchSetting&&) = default;
    LaunchSetting& operator=(LaunchSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["pre_node"]) {
            pre_node_.clear();
            for(auto& item : auto_yaml_node["pre_node"]) {
                pre_node_.push_back(item.as<std::string>());
            }
        }
        if(auto_yaml_node["delay"]) delay_ = auto_yaml_node["delay"].as<double>();
    }

        std::vector<std::string> PreNode() const {
            return pre_node_;
        }

        double Delay() const {
            return delay_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "LaunchSetting:" << std::endl;
        std::cout << indent << "    pre_node_: [" << std::endl;
        for(const auto& item : pre_node_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    delay_: " << delay_ << std::endl;
    }

private:
        std::vector<std::string> pre_node_;
        double delay_;
};

} // namespace auto_LaunchSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
namespace auto_ResidentGroup {
class ResidentGroup {
public:
    ResidentGroup() = default;
    ~ResidentGroup() = default;

    ResidentGroup(const ResidentGroup&) = default;
    ResidentGroup& operator=(const ResidentGroup&) = default;

    ResidentGroup(ResidentGroup&&) = default;
    ResidentGroup& operator=(ResidentGroup&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
        if(auto_yaml_node["node_list"]) {
            node_list_.clear();
            for(auto& item : auto_yaml_node["node_list"]) {
                auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList elem;
                elem.update_from_yaml(item);
                node_list_.push_back(elem);
            }
        }
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
        if(auto_yaml_node["launch_setting"]) launch_setting_.update_from_yaml(auto_yaml_node["launch_setting"]);
    }

        std::string TaskName() const {
            return task_name_;
        }

        std::vector<auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> NodeList() const {
            return node_list_;
        }

        const auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

        const auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting& LaunchSetting() const {
            return launch_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "ResidentGroup:" << std::endl;
        std::cout << indent << "    task_name_: " << task_name_ << std::endl;
        std::cout << indent << "    node_list_: [" << std::endl;
        for(const auto& item : node_list_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
        std::cout << indent << "    launch_setting_:" << std::endl;
        launch_setting_.print(indent_level + 1);
    }

private:
        std::string task_name_;
        std::vector<auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> node_list_;
        auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting system_setting_;
        auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting launch_setting_;
};

} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
namespace auto_TaskList {
class TaskList {
public:
    TaskList() = default;
    ~TaskList() = default;

    TaskList(const TaskList&) = default;
    TaskList& operator=(const TaskList&) = default;

    TaskList(TaskList&&) = default;
    TaskList& operator=(TaskList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["resident_group"]) {
            resident_group_.clear();
            for(auto& item : auto_yaml_node["resident_group"]) {
                auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup elem;
                elem.update_from_yaml(item);
                resident_group_.push_back(elem);
            }
        }
    }

        std::vector<auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup> ResidentGroup() const {
            return resident_group_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskList:" << std::endl;
        std::cout << indent << "    resident_group_: [" << std::endl;
        for(const auto& item : resident_group_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        std::vector<auto_Task::auto_TaskConfig::auto_TaskList::auto_ResidentGroup::ResidentGroup> resident_group_;
};

} // namespace auto_TaskList
} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig {
class TaskConfig {
public:
    TaskConfig() = default;
    ~TaskConfig() = default;

    TaskConfig(const TaskConfig&) = default;
    TaskConfig& operator=(const TaskConfig&) = default;

    TaskConfig(TaskConfig&&) = default;
    TaskConfig& operator=(TaskConfig&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["executer"]) executer_.update_from_yaml(auto_yaml_node["executer"]);
        if(auto_yaml_node["task_list"]) task_list_.update_from_yaml(auto_yaml_node["task_list"]);
    }

        const auto_Task::auto_TaskConfig::auto_Executer::Executer& Executer() const {
            return executer_;
        }

        const auto_Task::auto_TaskConfig::auto_TaskList::TaskList& TaskList() const {
            return task_list_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskConfig:" << std::endl;
        std::cout << indent << "    executer_:" << std::endl;
        executer_.print(indent_level + 1);
        std::cout << indent << "    task_list_:" << std::endl;
        task_list_.print(indent_level + 1);
    }

private:
        auto_Task::auto_TaskConfig::auto_Executer::Executer executer_;
        auto_Task::auto_TaskConfig::auto_TaskList::TaskList task_list_;
};

} // namespace auto_TaskConfig
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_Executer {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_Executer {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["executer_cpu_affinity"]) {
            executer_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["executer_cpu_affinity"]) {
                executer_cpu_affinity_.push_back(item.as<double>());
            }
        }
        if(auto_yaml_node["idle_task_cpu_affinity"]) {
            idle_task_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["idle_task_cpu_affinity"]) {
                idle_task_cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> ExecuterCpuAffinity() const {
            return executer_cpu_affinity_;
        }

        std::vector<double> IdleTaskCpuAffinity() const {
            return idle_task_cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    executer_cpu_affinity_: [" << std::endl;
        for(const auto& item : executer_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    idle_task_cpu_affinity_: [" << std::endl;
        for(const auto& item : idle_task_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> executer_cpu_affinity_;
        std::vector<double> idle_task_cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_Executer {
class Executer {
public:
    Executer() = default;
    ~Executer() = default;

    Executer(const Executer&) = default;
    Executer& operator=(const Executer&) = default;

    Executer(Executer&&) = default;
    Executer& operator=(Executer&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["package_name"]) package_name_ = auto_yaml_node["package_name"].as<std::string>();
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    }

        std::string PackageName() const {
            return package_name_;
        }

        const auto_Task::auto_TaskConfig2::auto_Executer::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task::auto_TaskConfig2::auto_Executer::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "Executer:" << std::endl;
        std::cout << indent << "    package_name_: " << package_name_ << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
    }

private:
        std::string package_name_;
        auto_Task::auto_TaskConfig2::auto_Executer::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task::auto_TaskConfig2::auto_Executer::auto_SystemSetting::SystemSetting system_setting_;
};

} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_NodeList {
class NodeList {
public:
    NodeList() = default;
    ~NodeList() = default;

    NodeList(const NodeList&) = default;
    NodeList& operator=(const NodeList&) = default;

    NodeList(NodeList&&) = default;
    NodeList& operator=(NodeList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
        if(auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
    }

        std::string NodeName() const {
            return node_name_;
        }

        bool OutputEnable() const {
            return output_enable_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "NodeList:" << std::endl;
        std::cout << indent << "    node_name_: " << node_name_ << std::endl;
        std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
    }

private:
        std::string node_name_;
        bool output_enable_;
};

} // namespace auto_NodeList
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["cpu_affinity"]) {
            cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["cpu_affinity"]) {
                cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> CpuAffinity() const {
            return cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    cpu_affinity_: [" << std::endl;
        for(const auto& item : cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_LaunchSetting {
class LaunchSetting {
public:
    LaunchSetting() = default;
    ~LaunchSetting() = default;

    LaunchSetting(const LaunchSetting&) = default;
    LaunchSetting& operator=(const LaunchSetting&) = default;

    LaunchSetting(LaunchSetting&&) = default;
    LaunchSetting& operator=(LaunchSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["pre_node"]) {
            pre_node_.clear();
            for(auto& item : auto_yaml_node["pre_node"]) {
                pre_node_.push_back(item.as<std::string>());
            }
        }
        if(auto_yaml_node["delay"]) delay_ = auto_yaml_node["delay"].as<double>();
    }

        std::vector<std::string> PreNode() const {
            return pre_node_;
        }

        double Delay() const {
            return delay_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "LaunchSetting:" << std::endl;
        std::cout << indent << "    pre_node_: [" << std::endl;
        for(const auto& item : pre_node_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    delay_: " << delay_ << std::endl;
    }

private:
        std::vector<std::string> pre_node_;
        double delay_;
};

} // namespace auto_LaunchSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
class ResidentGroup {
public:
    ResidentGroup() = default;
    ~ResidentGroup() = default;

    ResidentGroup(const ResidentGroup&) = default;
    ResidentGroup& operator=(const ResidentGroup&) = default;

    ResidentGroup(ResidentGroup&&) = default;
    ResidentGroup& operator=(ResidentGroup&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
        if(auto_yaml_node["node_list"]) {
            node_list_.clear();
            for(auto& item : auto_yaml_node["node_list"]) {
                auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList elem;
                elem.update_from_yaml(item);
                node_list_.push_back(elem);
            }
        }
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
        if(auto_yaml_node["launch_setting"]) launch_setting_.update_from_yaml(auto_yaml_node["launch_setting"]);
    }

        std::string TaskName() const {
            return task_name_;
        }

        std::vector<auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> NodeList() const {
            return node_list_;
        }

        const auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

        const auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting& LaunchSetting() const {
            return launch_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "ResidentGroup:" << std::endl;
        std::cout << indent << "    task_name_: " << task_name_ << std::endl;
        std::cout << indent << "    node_list_: [" << std::endl;
        for(const auto& item : node_list_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
        std::cout << indent << "    launch_setting_:" << std::endl;
        launch_setting_.print(indent_level + 1);
    }

private:
        std::string task_name_;
        std::vector<auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> node_list_;
        auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting system_setting_;
        auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting launch_setting_;
};

} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
class TaskList {
public:
    TaskList() = default;
    ~TaskList() = default;

    TaskList(const TaskList&) = default;
    TaskList& operator=(const TaskList&) = default;

    TaskList(TaskList&&) = default;
    TaskList& operator=(TaskList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["resident_group"]) {
            resident_group_.clear();
            for(auto& item : auto_yaml_node["resident_group"]) {
                auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup elem;
                elem.update_from_yaml(item);
                resident_group_.push_back(elem);
            }
        }
    }

        std::vector<auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup> ResidentGroup() const {
            return resident_group_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskList:" << std::endl;
        std::cout << indent << "    resident_group_: [" << std::endl;
        for(const auto& item : resident_group_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        std::vector<auto_Task::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup> resident_group_;
};

} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task {
namespace auto_TaskConfig2 {
class TaskConfig2 {
public:
    TaskConfig2() = default;
    ~TaskConfig2() = default;

    TaskConfig2(const TaskConfig2&) = default;
    TaskConfig2& operator=(const TaskConfig2&) = default;

    TaskConfig2(TaskConfig2&&) = default;
    TaskConfig2& operator=(TaskConfig2&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["executer"]) executer_.update_from_yaml(auto_yaml_node["executer"]);
        if(auto_yaml_node["task_list"]) task_list_.update_from_yaml(auto_yaml_node["task_list"]);
    }

        const auto_Task::auto_TaskConfig2::auto_Executer::Executer& Executer() const {
            return executer_;
        }

        const auto_Task::auto_TaskConfig2::auto_TaskList::TaskList& TaskList() const {
            return task_list_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskConfig2:" << std::endl;
        std::cout << indent << "    executer_:" << std::endl;
        executer_.print(indent_level + 1);
        std::cout << indent << "    task_list_:" << std::endl;
        task_list_.print(indent_level + 1);
    }

private:
        auto_Task::auto_TaskConfig2::auto_Executer::Executer executer_;
        auto_Task::auto_TaskConfig2::auto_TaskList::TaskList task_list_;
};

} // namespace auto_TaskConfig2
} // namespace auto_Task

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_Executer {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_Executer {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["executer_cpu_affinity"]) {
            executer_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["executer_cpu_affinity"]) {
                executer_cpu_affinity_.push_back(item.as<double>());
            }
        }
        if(auto_yaml_node["idle_task_cpu_affinity"]) {
            idle_task_cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["idle_task_cpu_affinity"]) {
                idle_task_cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> ExecuterCpuAffinity() const {
            return executer_cpu_affinity_;
        }

        std::vector<double> IdleTaskCpuAffinity() const {
            return idle_task_cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    executer_cpu_affinity_: [" << std::endl;
        for(const auto& item : executer_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    idle_task_cpu_affinity_: [" << std::endl;
        for(const auto& item : idle_task_cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> executer_cpu_affinity_;
        std::vector<double> idle_task_cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_Executer {
class Executer {
public:
    Executer() = default;
    ~Executer() = default;

    Executer(const Executer&) = default;
    Executer& operator=(const Executer&) = default;

    Executer(Executer&&) = default;
    Executer& operator=(Executer&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["package_name"]) package_name_ = auto_yaml_node["package_name"].as<std::string>();
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
    }

        std::string PackageName() const {
            return package_name_;
        }

        const auto_Task2::auto_TaskConfig2::auto_Executer::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task2::auto_TaskConfig2::auto_Executer::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "Executer:" << std::endl;
        std::cout << indent << "    package_name_: " << package_name_ << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
    }

private:
        std::string package_name_;
        auto_Task2::auto_TaskConfig2::auto_Executer::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task2::auto_TaskConfig2::auto_Executer::auto_SystemSetting::SystemSetting system_setting_;
};

} // namespace auto_Executer
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_NodeList {
class NodeList {
public:
    NodeList() = default;
    ~NodeList() = default;

    NodeList(const NodeList&) = default;
    NodeList& operator=(const NodeList&) = default;

    NodeList(NodeList&&) = default;
    NodeList& operator=(NodeList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["node_name"]) node_name_ = auto_yaml_node["node_name"].as<std::string>();
        if(auto_yaml_node["output_enable"]) output_enable_ = auto_yaml_node["output_enable"].as<bool>();
    }

        std::string NodeName() const {
            return node_name_;
        }

        bool OutputEnable() const {
            return output_enable_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "NodeList:" << std::endl;
        std::cout << indent << "    node_name_: " << node_name_ << std::endl;
        std::cout << indent << "    output_enable_: " << output_enable_ << std::endl;
    }

private:
        std::string node_name_;
        bool output_enable_;
};

} // namespace auto_NodeList
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_TaskSetting {
class TaskSetting {
public:
    TaskSetting() = default;
    ~TaskSetting() = default;

    TaskSetting(const TaskSetting&) = default;
    TaskSetting& operator=(const TaskSetting&) = default;

    TaskSetting(TaskSetting&&) = default;
    TaskSetting& operator=(TaskSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["timer_type"]) timer_type_ = auto_yaml_node["timer_type"].as<std::string>();
        if(auto_yaml_node["period"]) period_ = auto_yaml_node["period"].as<double>();
    }

        std::string TimerType() const {
            return timer_type_;
        }

        double Period() const {
            return period_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskSetting:" << std::endl;
        std::cout << indent << "    timer_type_: " << timer_type_ << std::endl;
        std::cout << indent << "    period_: " << period_ << std::endl;
    }

private:
        std::string timer_type_;
        double period_;
};

} // namespace auto_TaskSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_SystemSetting {
class SystemSetting {
public:
    SystemSetting() = default;
    ~SystemSetting() = default;

    SystemSetting(const SystemSetting&) = default;
    SystemSetting& operator=(const SystemSetting&) = default;

    SystemSetting(SystemSetting&&) = default;
    SystemSetting& operator=(SystemSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["priority"]) priority_ = auto_yaml_node["priority"].as<double>();
        if(auto_yaml_node["cpu_affinity"]) {
            cpu_affinity_.clear();
            for(auto& item : auto_yaml_node["cpu_affinity"]) {
                cpu_affinity_.push_back(item.as<double>());
            }
        }
    }

        double Priority() const {
            return priority_;
        }

        std::vector<double> CpuAffinity() const {
            return cpu_affinity_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "SystemSetting:" << std::endl;
        std::cout << indent << "    priority_: " << priority_ << std::endl;
        std::cout << indent << "    cpu_affinity_: [" << std::endl;
        for(const auto& item : cpu_affinity_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        double priority_;
        std::vector<double> cpu_affinity_;
};

} // namespace auto_SystemSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
namespace auto_LaunchSetting {
class LaunchSetting {
public:
    LaunchSetting() = default;
    ~LaunchSetting() = default;

    LaunchSetting(const LaunchSetting&) = default;
    LaunchSetting& operator=(const LaunchSetting&) = default;

    LaunchSetting(LaunchSetting&&) = default;
    LaunchSetting& operator=(LaunchSetting&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["pre_node"]) {
            pre_node_.clear();
            for(auto& item : auto_yaml_node["pre_node"]) {
                pre_node_.push_back(item.as<std::string>());
            }
        }
        if(auto_yaml_node["delay"]) delay_ = auto_yaml_node["delay"].as<double>();
    }

        std::vector<std::string> PreNode() const {
            return pre_node_;
        }

        double Delay() const {
            return delay_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "LaunchSetting:" << std::endl;
        std::cout << indent << "    pre_node_: [" << std::endl;
        for(const auto& item : pre_node_) {
            std::cout << indent << "        " << item << std::endl;
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    delay_: " << delay_ << std::endl;
    }

private:
        std::vector<std::string> pre_node_;
        double delay_;
};

} // namespace auto_LaunchSetting
} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
namespace auto_ResidentGroup {
class ResidentGroup {
public:
    ResidentGroup() = default;
    ~ResidentGroup() = default;

    ResidentGroup(const ResidentGroup&) = default;
    ResidentGroup& operator=(const ResidentGroup&) = default;

    ResidentGroup(ResidentGroup&&) = default;
    ResidentGroup& operator=(ResidentGroup&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["task_name"]) task_name_ = auto_yaml_node["task_name"].as<std::string>();
        if(auto_yaml_node["node_list"]) {
            node_list_.clear();
            for(auto& item : auto_yaml_node["node_list"]) {
                auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList elem;
                elem.update_from_yaml(item);
                node_list_.push_back(elem);
            }
        }
        if(auto_yaml_node["task_setting"]) task_setting_.update_from_yaml(auto_yaml_node["task_setting"]);
        if(auto_yaml_node["system_setting"]) system_setting_.update_from_yaml(auto_yaml_node["system_setting"]);
        if(auto_yaml_node["launch_setting"]) launch_setting_.update_from_yaml(auto_yaml_node["launch_setting"]);
    }

        std::string TaskName() const {
            return task_name_;
        }

        std::vector<auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> NodeList() const {
            return node_list_;
        }

        const auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting& TaskSetting() const {
            return task_setting_;
        }

        const auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting& SystemSetting() const {
            return system_setting_;
        }

        const auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting& LaunchSetting() const {
            return launch_setting_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "ResidentGroup:" << std::endl;
        std::cout << indent << "    task_name_: " << task_name_ << std::endl;
        std::cout << indent << "    node_list_: [" << std::endl;
        for(const auto& item : node_list_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
        std::cout << indent << "    task_setting_:" << std::endl;
        task_setting_.print(indent_level + 1);
        std::cout << indent << "    system_setting_:" << std::endl;
        system_setting_.print(indent_level + 1);
        std::cout << indent << "    launch_setting_:" << std::endl;
        launch_setting_.print(indent_level + 1);
    }

private:
        std::string task_name_;
        std::vector<auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_NodeList::NodeList> node_list_;
        auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_TaskSetting::TaskSetting task_setting_;
        auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_SystemSetting::SystemSetting system_setting_;
        auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::auto_LaunchSetting::LaunchSetting launch_setting_;
};

} // namespace auto_ResidentGroup
} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
namespace auto_TaskList {
class TaskList {
public:
    TaskList() = default;
    ~TaskList() = default;

    TaskList(const TaskList&) = default;
    TaskList& operator=(const TaskList&) = default;

    TaskList(TaskList&&) = default;
    TaskList& operator=(TaskList&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["resident_group"]) {
            resident_group_.clear();
            for(auto& item : auto_yaml_node["resident_group"]) {
                auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup elem;
                elem.update_from_yaml(item);
                resident_group_.push_back(elem);
            }
        }
    }

        std::vector<auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup> ResidentGroup() const {
            return resident_group_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskList:" << std::endl;
        std::cout << indent << "    resident_group_: [" << std::endl;
        for(const auto& item : resident_group_) {
            item.print(indent_level + 2);
        }
        std::cout << indent << "    ]" << std::endl;
    }

private:
        std::vector<auto_Task2::auto_TaskConfig2::auto_TaskList::auto_ResidentGroup::ResidentGroup> resident_group_;
};

} // namespace auto_TaskList
} // namespace auto_TaskConfig2
} // namespace auto_Task2

namespace auto_Task2 {
namespace auto_TaskConfig2 {
class TaskConfig2 {
public:
    TaskConfig2() = default;
    ~TaskConfig2() = default;

    TaskConfig2(const TaskConfig2&) = default;
    TaskConfig2& operator=(const TaskConfig2&) = default;

    TaskConfig2(TaskConfig2&&) = default;
    TaskConfig2& operator=(TaskConfig2&&) = default;

    void update_from_yaml(const YAML::Node& auto_yaml_node) {
        if(auto_yaml_node["executer"]) executer_.update_from_yaml(auto_yaml_node["executer"]);
        if(auto_yaml_node["task_list"]) task_list_.update_from_yaml(auto_yaml_node["task_list"]);
    }

        const auto_Task2::auto_TaskConfig2::auto_Executer::Executer& Executer() const {
            return executer_;
        }

        const auto_Task2::auto_TaskConfig2::auto_TaskList::TaskList& TaskList() const {
            return task_list_;
        }

    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "TaskConfig2:" << std::endl;
        std::cout << indent << "    executer_:" << std::endl;
        executer_.print(indent_level + 1);
        std::cout << indent << "    task_list_:" << std::endl;
        task_list_.print(indent_level + 1);
    }

private:
        auto_Task2::auto_TaskConfig2::auto_Executer::Executer executer_;
        auto_Task2::auto_TaskConfig2::auto_TaskList::TaskList task_list_;
};

} // namespace auto_TaskConfig2
} // namespace auto_Task2

class ConfigCollect {
public:
    // 获取单例实例
    static ConfigCollect& getInstance() {
        static ConfigCollect instance;
        return instance;
    }

    // 删除拷贝构造函数和赋值操作符
    ConfigCollect(const ConfigCollect&) = delete;
    ConfigCollect& operator=(const ConfigCollect&) = delete;

    // 删除移动构造函数和赋值操作符
    ConfigCollect(ConfigCollect&&) = delete;
    ConfigCollect& operator=(ConfigCollect&&) = delete;

    // 根据名称更新对应的配置
    void update_from_yaml(const std::string& name, const std::string& base_path) {
        if (name.empty()) {
            return;
        }
        bool matched = false;
        if (name == "task_task_config") {
            update_from_yaml_task_task_config(base_path);
            matched = true;
        }
        else if (name == "task_task_config2") {
            update_from_yaml_task_task_config2(base_path);
            matched = true;
        }
        else if (name == "task2_task_config2") {
            update_from_yaml_task2_task_config2(base_path);
            matched = true;
        }
        if (!matched) {
            // 未找到匹配的配置项
            std::cerr << "[ConfigCollect] No matching configuration for '" << name << "'" << std::endl;
        }
    }

    // 更新所有配置
    void update_from_yaml_all(const std::string& base_path) {
        update_from_yaml_task_task_config(base_path);
        update_from_yaml_task_task_config2(base_path);
        update_from_yaml_task2_task_config2(base_path);
    }

    // 打印所有配置
    void print(int indent_level = 0) const {
        std::string indent(indent_level * 4, ' ');
        std::cout << indent << "task_task_config:" << std::endl;
        {
            std::shared_lock<std::shared_mutex> lock(m_task_task_config);
            task_task_config.print(indent_level + 1);
        }

        std::cout << indent << "task_task_config2:" << std::endl;
        {
            std::shared_lock<std::shared_mutex> lock(m_task_task_config2);
            task_task_config2.print(indent_level + 1);
        }

        std::cout << indent << "task2_task_config2:" << std::endl;
        {
            std::shared_lock<std::shared_mutex> lock(m_task2_task_config2);
            task2_task_config2.print(indent_level + 1);
        }

    }

    // 获取各个配置的实例
    auto_Task::auto_TaskConfig::TaskConfig& get_task_task_config() {
        std::shared_lock<std::shared_mutex> lock(m_task_task_config);
        return task_task_config;
    }

    auto_Task::auto_TaskConfig2::TaskConfig2& get_task_task_config2() {
        std::shared_lock<std::shared_mutex> lock(m_task_task_config2);
        return task_task_config2;
    }

    auto_Task2::auto_TaskConfig2::TaskConfig2& get_task2_task_config2() {
        std::shared_lock<std::shared_mutex> lock(m_task2_task_config2);
        return task2_task_config2;
    }

private:
    // 私有构造函数，防止外部实例化
    ConfigCollect() = default;

    auto_Task::auto_TaskConfig::TaskConfig task_task_config;
    mutable std::shared_mutex m_task_task_config;

    // 更新 auto_Task::auto_TaskConfig::TaskConfig 配置
    void update_from_yaml_task_task_config(const std::string& base_path) {
        YAML::Node auto_yaml_node;
        // 加载 YAML 文件
        auto_yaml_node = YAML::LoadFile(base_path + "/task/task_config.yaml");
        {
            std::unique_lock<std::shared_mutex> lock(m_task_task_config);
            task_task_config.update_from_yaml(auto_yaml_node);
        }
    }

    auto_Task::auto_TaskConfig2::TaskConfig2 task_task_config2;
    mutable std::shared_mutex m_task_task_config2;

    // 更新 auto_Task::auto_TaskConfig2::TaskConfig2 配置
    void update_from_yaml_task_task_config2(const std::string& base_path) {
        YAML::Node auto_yaml_node;
        // 加载 YAML 文件
        auto_yaml_node = YAML::LoadFile(base_path + "/task/task_config2.yaml");
        {
            std::unique_lock<std::shared_mutex> lock(m_task_task_config2);
            task_task_config2.update_from_yaml(auto_yaml_node);
        }
    }

    auto_Task2::auto_TaskConfig2::TaskConfig2 task2_task_config2;
    mutable std::shared_mutex m_task2_task_config2;

    // 更新 auto_Task2::auto_TaskConfig2::TaskConfig2 配置
    void update_from_yaml_task2_task_config2(const std::string& base_path) {
        YAML::Node auto_yaml_node;
        // 加载 YAML 文件
        auto_yaml_node = YAML::LoadFile(base_path + "/task2/task_config2.yaml");
        {
            std::unique_lock<std::shared_mutex> lock(m_task2_task_config2);
            task2_task_config2.update_from_yaml(auto_yaml_node);
        }
    }

};

}
