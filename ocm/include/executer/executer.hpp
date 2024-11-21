#pragma once
#include <log_anywhere/log_anywhere.hpp>
#include <set>
#include <unordered_map>
#include <vector>
#include "common/struct_type.hpp"
#include "node/node.hpp"
#include "node/node_map.hpp"
#include "ocm/ocm.hpp"
#include "task/task.hpp"

namespace openrobot::ocm {

class Task : public TaskBase {
 public:
  Task(const TaskSetting& task_setting, const std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>>& node_list)
      : TaskBase(task_setting.task_name, task_setting.timer_setting.timer_type, static_cast<double>(task_setting.launch_setting.delay),
                 task_setting.task_name),
        task_setting_(task_setting),
        node_list_(node_list) {
    SetPeriod(task_setting_.timer_setting.period);
  }
  ~Task() = default;
  void Init() {
    for (auto& node : *node_list_) {
      node->Init();
      node->RunOnce();
    }
  }
  void Run() override {
    std::cout << "Task " << GetTaskName() << ":" << GetLoopDuration() << std::endl;
    for (auto& node : *node_list_) {
      node->RunOnce();
    }
  }

  const TaskSetting& GetTaskSetting() const { return task_setting_; }

 private:
  TaskSetting task_setting_;
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};
class Executer : public TaskBase {
 public:
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map)
      : TaskBase(executer_config.executer_setting.package_name, executer_config.executer_setting.timer_setting.timer_type, 0.0,
                 executer_config.executer_setting.sem_name),
        node_map_(node_map),
        executer_config_(executer_config) {
    SetPeriod(executer_config_.executer_setting.timer_setting.period);
    TaskStart();
    CreateTask();
    current_group_ = "empty_init";
    target_group_ = "empty_init";
    desired_group_ = "group3";
    is_transition_ = false;
    all_node_exit_check_ = false;
    all_node_enter_check_ = false;
    task_stop_flag_ = true;
    task_start_flag_ = true;
    all_current_task_stop_ = false;
  }
  ~Executer() = default;

  void Init() {
    std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
    std::set<std::string> task_set_wait_to_start;
    for (auto& task : resident_group_task_list_) {
      for (auto& task_ptr : task.second) {
        task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr));
        task_set_wait_to_start.insert(task_ptr->GetTaskName());
      }
    }
    while (!task_set_wait_to_start.empty()) {
      for (auto& task : task_list_wait_to_start) {
        if (!task.first) {
          bool is_pre_node_empty = task.second->GetTaskSetting().launch_setting.pre_node.empty();
          const auto& pre_node_list = task.second->GetTaskSetting().launch_setting.pre_node;
          bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
            return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
          });
          if (is_pre_node_empty || is_pre_node_ready) {
            task.first = true;
            task.second->Init();
            task.second->TaskStart();
            task_set_wait_to_start.erase(task.second->GetTaskName());
            logger.info("Executer: Task {} start.", task.second->GetTaskName());
          }
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  AtomicPtr<std::string> desired_group_;
  AtomicPtr<std::string> current_group_;

 private:
  void Run() override {
    TransitionCheck();
    if (is_transition_) {
      Transition();
    }
  }
  void CreateTask() {
    for (auto& task_config : executer_config_.task_list.resident_group) {
      std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
      for (auto& node_config : task_config.second.node_list) {
        node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
      }
      resident_group_task_list_[task_config.second.task_name].emplace_back(std::make_shared<Task>(task_config.second, node_list));
    }
    for (auto& task_config : executer_config_.task_list.standby_group) {
      std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
      for (auto& node_config : task_config.second.node_list) {
        node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
      }
      standby_group_task_list_[task_config.second.task_name].emplace_back(std::make_shared<Task>(task_config.second, node_list));
    }
  }

  void TransitionCheck() {
    // if (!is_transition_) {
    //   const auto& desired_group = desired_group_.GetValue();
    //   const auto& current_group = current_group_.GetValue();
    //   if (desired_group != current_group) {
    //     if (exclusive_group_set_.find(desired_group) != exclusive_group_set_.end()) {
    //       target_task_set_.clear();
    //       current_task_set_.clear();
    //       target_node_set_.clear();
    //       current_node_set_.clear();
    //       enter_node_set_.clear();
    //       exit_node_set_.clear();
    //       target_group_ = desired_group;
    //       const auto& exclusive_group_config = executer_config_.exclusive_group;
    //       for (auto& task_config : exclusive_group_config.at(target_group_).task_list) {
    //         for (auto& task : standby_group_task_list_.at(task_config.task_name)) {
    //           target_task_set_.insert(task);
    //         }
    //         for (auto& node : task_config.node_list) {
    //           target_node_set_.insert(node.node_name);
    //         }
    //       }
    //       if (current_group != "empty_init") {
    //         for (auto& task_config : exclusive_group_config.at(current_group).task_list) {
    //           for (auto& task : standby_group_task_list_.at(task_config.task_name)) {
    //             current_task_set_.insert(task);
    //           }
    //           for (auto& node : task_config.node_list) {
    //             current_node_set_.insert(node.node_name);
    //           }
    //         }
    //       }
    //       std::set_difference(current_node_set_.begin(), current_node_set_.end(), target_node_set_.begin(), target_node_set_.end(),
    //                           std::inserter(exit_node_set_, exit_node_set_.begin()));

    //       std::set_difference(target_node_set_.begin(), target_node_set_.end(), current_node_set_.begin(), current_node_set_.end(),
    //                           std::inserter(enter_node_set_, enter_node_set_.begin()));
    //       all_node_exit_check_ = false;
    //       all_node_enter_check_ = false;
    //       is_transition_ = true;
    //       task_stop_flag_ = true;
    //       task_start_flag_ = true;
    //       all_current_task_stop_ = false;
    //     } else {
    //       logger.error("Executer: Target group {} is not an exclusive group.", desired_group);
    //     }
    //   }
    // }
  }
  void Transition() {
    // if (all_node_exit_check_ && all_node_enter_check_) {
    //   if (task_stop_flag_) {
    //     task_stop_flag_ = false;
    //     for (auto& task : current_task_set_) {
    //       task->TaskStop();
    //     }
    //   }
    //   if (all_current_task_stop_) {
    //     std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
    //     std::set<std::string> task_set_wait_to_start;

    //     for (auto& task_config : executer_config_.exclusive_group.at(target_group_).task_list) {
    //       auto& task_vector = standby_group_task_list_.at(task_config.task_name);
    //       for (auto& task : task_vector) {
    //         task_list_wait_to_start.emplace_back(std::make_pair(false, task));
    //         task_set_wait_to_start.insert(task->GetTaskName());
    //       }
    //     }
    //     while (!task_set_wait_to_start.empty()) {
    //       for (auto& task : task_list_wait_to_start) {
    //         if (!task.first) {
    //           bool is_pre_node_empty = task.second->GetTaskConfig().launch_setting.pre_node.empty();
    //           const auto& pre_node_list = task.second->GetTaskConfig().launch_setting.pre_node;
    //           bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
    //             return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
    //           });
    //           if (is_pre_node_empty || is_pre_node_ready) {
    //             task.first = true;
    //             task.second->Init();
    //             task.second->TaskStart();
    //             task_set_wait_to_start.erase(task.second->GetTaskName());
    //             logger.info("Executer: Task {} start.", task.second->GetTaskName());
    //           }
    //         }
    //       }
    //     }
    //     current_group_ = target_group_;
    //     is_transition_ = false;
    //   } else {
    //     all_current_task_stop_ =
    //         std::all_of(current_task_set_.begin(), current_task_set_.end(), [](const auto& task) { return task->GetState() == TaskState::STANDBY;
    //         });
    //   }
    // } else {
    //   all_node_exit_check_ =
    //       std::all_of(exit_node_set_.begin(), exit_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->ExitCheck(); });
    //   all_node_enter_check_ =
    //       std::all_of(enter_node_set_.begin(), enter_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->EnterCheck();
    //       });
    // }
  }
  std::unordered_map<std::string, std::vector<std::shared_ptr<Task>>> resident_group_task_list_;
  std::unordered_map<std::string, std::vector<std::shared_ptr<Task>>> standby_group_task_list_;
  std::set<std::string> exclusive_group_set_;
  std::shared_ptr<NodeMap> node_map_;
  ExecuterConfig executer_config_;

  std::set<std::shared_ptr<Task>> target_task_set_;
  std::set<std::shared_ptr<Task>> current_task_set_;
  std::set<std::string> target_node_set_;
  std::set<std::string> current_node_set_;
  std::set<std::string> enter_node_set_;
  std::set<std::string> exit_node_set_;
  bool all_node_exit_check_;
  bool all_node_enter_check_;
  bool task_stop_flag_;
  bool task_start_flag_;
  bool all_current_task_stop_;
  std::string target_group_;
  bool is_transition_;

  openrobot::ocm::LogAnywhere& logger = openrobot::ocm::LogAnywhere::getInstance();
};
}  // namespace openrobot::ocm
