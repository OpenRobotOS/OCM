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
    for (const auto& node : task_setting_.node_list) {
      node_output_flag_[node.node_name] = node.output_enable;
      node_init_flag_[node.node_name] = false;
    }
  }
  ~Task() = default;

  void Init() {
    for (auto& node : node_init_flag_) {
      node.second = true;
    }
    InitNode(node_init_flag_);
  }
  std::set<std::string> Init(const std::set<std::string>& init_node_list) {
    std::set<std::string> init_node_list_result;
    for (auto& node : node_init_flag_) {
      if (init_node_list.find(node.first) != init_node_list.end()) {
        node.second = true;
        init_node_list_result.insert(node.first);
      }
    }
    InitNode(node_init_flag_);
    return init_node_list_result;
  }
  void Run() override {
    // std::cout << "Task " << GetTaskName() << ":" << GetLoopDuration() << std::endl;
    for (auto& node : *node_list_) {
      const auto& node_name = node->GetNodeName();
      if (!node_init_flag_[node_name]) {
        node->RunOnce();
        if (node_output_flag_[node_name]) {
          node->Output();
        }
      } else {
        node_init_flag_.at(node_name) = false;
      }
    }
  }

  const TaskSetting& GetTaskSetting() const { return task_setting_; }

 private:
  void InitNode(const std::unordered_map<std::string, bool>& node_init_flag) {
    for (auto& node : *node_list_) {
      if (node_init_flag.at(node->GetNodeName())) {
        node->Init();
        node->RunOnce();
        if (node_output_flag_[node->GetNodeName()]) {
          node->Output();
        }
      }
    }
  }
  std::unordered_map<std::string, bool> node_init_flag_;
  TaskSetting task_setting_;
  std::unordered_map<std::string, bool> node_output_flag_;
  std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list_;
};
class Executer : public TaskBase {
 public:
  Executer(const ExecuterConfig& executer_config, const std::shared_ptr<NodeMap>& node_map)
      : TaskBase(executer_config.executer_setting.package_name, executer_config.executer_setting.timer_setting.timer_type, 0.0,
                 executer_config.executer_setting.sem_name),
        node_map_(node_map),
        executer_config_(executer_config) {
    logger_ = spdlog::get("openrobot_ocm_logger");
    SetPeriod(executer_config_.executer_setting.timer_setting.period);
    TaskStart();
    current_group_ = "empty_init";
    target_group_ = "empty_init";
    desired_group_history_ = "empty_init";
    desired_group_ = "group3";
    is_transition_ = false;
    all_node_exit_check_ = false;
    all_node_enter_check_ = false;
    task_stop_flag_ = true;
    task_start_flag_ = true;
    all_current_task_stop_ = false;
  }
  ~Executer() = default;

  void CreateTask() {
    for (auto& task_setting : executer_config_.task_list.resident_group) {
      std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
      for (auto& node_config : task_setting.second.node_list) {
        node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
      }
      resident_group_task_list_[task_setting.second.task_name] = std::make_shared<Task>(task_setting.second, node_list);
      logger_->debug("Executer: Task {} added.", task_setting.second.task_name);
    }
    for (auto& task_setting : executer_config_.task_list.standby_group) {
      std::shared_ptr<std::vector<std::shared_ptr<NodeBase>>> node_list = std::make_shared<std::vector<std::shared_ptr<NodeBase>>>();
      for (auto& node_config : task_setting.second.node_list) {
        node_list->emplace_back(node_map_->GetNodePtr(node_config.node_name));
      }
      standby_group_task_list_[task_setting.second.task_name] = std::make_shared<Task>(task_setting.second, node_list);
      logger_->debug("Executer: Task {} added.", task_setting.second.task_name);
    }
    for (auto& exclusive_task_group : executer_config_.exclusive_task_group) {
      logger_->debug("Executer: Exclusive group {} added.", exclusive_task_group.second.group_name);
      exclusive_group_set_.insert(exclusive_task_group.second.group_name);
    }
  }

  void InitTask() {
    std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
    std::set<std::string> task_set_wait_to_start;
    for (auto& task_ptr : resident_group_task_list_) {
      task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr.second));
      task_set_wait_to_start.insert(task_ptr.second->GetTaskName());
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
            logger_->info("Executer: Task {} start.", task.second->GetTaskName());
          }
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  AtomicPtr<std::string> desired_group_;
  AtomicPtr<std::string> current_group_;

 private:
  std::shared_ptr<spdlog::logger> logger_;
  void Run() override {
    TransitionCheck();
    if (is_transition_) {
      Transition();
    }
  }

  void TransitionCheck() {
    if (!is_transition_) {
      const auto& desired_group = desired_group_.GetValue();
      const auto& current_group = current_group_.GetValue();
      if (desired_group != current_group) {
        if (exclusive_group_set_.find(desired_group) != exclusive_group_set_.end()) {
          target_task_set_.clear();
          current_task_set_.clear();
          target_node_set_.clear();
          current_node_set_.clear();
          enter_node_set_.clear();
          exit_node_set_.clear();
          const auto& exclusive_group_config = executer_config_.exclusive_task_group;
          for (auto& task_config : exclusive_group_config.at(desired_group).task_list) {
            const auto& task_name = task_config.second.task_name;
            target_task_set_.insert(standby_group_task_list_.at(task_name));
            for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
              target_node_set_.insert(node.node_name);
            }
          }
          if (current_group != "empty_init") {
            for (auto& task_config : exclusive_group_config.at(current_group).task_list) {
              const auto& task_name = task_config.second.task_name;
              current_task_set_.insert(standby_group_task_list_.at(task_name));
              for (auto& node : executer_config_.task_list.standby_group.at(task_name).node_list) {
                current_node_set_.insert(node.node_name);
              }
            }
          }
          std::set_difference(current_node_set_.begin(), current_node_set_.end(), target_node_set_.begin(), target_node_set_.end(),
                              std::inserter(exit_node_set_, exit_node_set_.begin()));

          std::set_difference(target_node_set_.begin(), target_node_set_.end(), current_node_set_.begin(), current_node_set_.end(),
                              std::inserter(enter_node_set_, enter_node_set_.begin()));
          all_node_exit_check_ = false;
          all_node_enter_check_ = false;
          is_transition_ = true;
          task_stop_flag_ = true;
          task_start_flag_ = true;
          all_current_task_stop_ = false;
          target_group_ = desired_group;
          logger_->info("Executer: Transition from group {} to group {}", ColorPrint(current_group, ColorEnum::YELLOW),
                        ColorPrint(desired_group, ColorEnum::YELLOW));
        } else {
          if (desired_group_history_ != desired_group) {
            desired_group_history_ = desired_group;
            logger_->error("Executer: Target group {} is not an exclusive group.", ColorPrint(desired_group, ColorEnum::RED));
          }
        }
      }
    }
  }
  void Transition() {
    if (all_node_exit_check_ && all_node_enter_check_) {
      if (task_stop_flag_) {
        task_stop_flag_ = false;
        for (auto& task : current_task_set_) {
          task->TaskStop();
        }
      }
      if (all_current_task_stop_) {
        std::vector<std::pair<bool, std::shared_ptr<Task>>> task_list_wait_to_start;
        std::set<std::string> task_set_wait_to_start;
        for (auto& task_ptr : target_task_set_) {
          task_list_wait_to_start.emplace_back(std::make_pair(false, task_ptr));
          task_set_wait_to_start.insert(task_ptr->GetTaskName());
        }
        std::set<std::string> all_init_node_set_log;
        while (!task_set_wait_to_start.empty()) {
          for (auto& task : task_list_wait_to_start) {
            const auto& task_name = task.second->GetTaskName();
            if (!task.first) {
              const auto& task_setting = executer_config_.exclusive_task_group.at(target_group_).task_list.at(task_name);
              bool is_pre_node_empty = task_setting.pre_node.empty();
              const auto& pre_node_list = task_setting.pre_node;
              const std::set<std::string> force_init_node_set(task_setting.force_init_node.begin(), task_setting.force_init_node.end());
              std::set<std::string> all_init_node_set;
              std::set_union(force_init_node_set.begin(), force_init_node_set.end(), enter_node_set_.begin(), enter_node_set_.end(),
                             std::inserter(all_init_node_set, all_init_node_set.begin()));
              bool is_pre_node_ready = std::all_of(pre_node_list.begin(), pre_node_list.end(), [this](const std::string& pre_node_name) {
                return node_map_->GetNodePtr(pre_node_name)->GetState() == NodeState::RUNNING;
              });
              if (is_pre_node_empty || is_pre_node_ready) {
                task.first = true;
                auto init_node_set = task.second->Init(all_init_node_set);
                task.second->TaskStart();
                all_init_node_set_log.insert(init_node_set.begin(), init_node_set.end());
                task_set_wait_to_start.erase(task.second->GetTaskName());
                logger_->info("Executer: Task {} start.", task.second->GetTaskName());
              }
            }
          }
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        current_group_ = target_group_;
        is_transition_ = false;

        logger_->info("Executer: Exit node: {}, Enter node: {}", JointStrSet(exit_node_set_, ","), JointStrSet(enter_node_set_, ","));
        logger_->info("Executer: Transition to group {} finished.", ColorPrint(target_group_, ColorEnum::YELLOW));
      } else {
        all_current_task_stop_ =
            std::all_of(current_task_set_.begin(), current_task_set_.end(), [](const auto& task) { return task->GetState() == TaskState::STANDBY; });
      }
    } else {
      all_node_exit_check_ =
          std::all_of(exit_node_set_.begin(), exit_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->ExitCheck(); });
      all_node_enter_check_ =
          std::all_of(enter_node_set_.begin(), enter_node_set_.end(), [this](const auto& node) { return node_map_->GetNodePtr(node)->EnterCheck(); });
    }
  }
  std::unordered_map<std::string, std::shared_ptr<Task>> resident_group_task_list_;
  std::unordered_map<std::string, std::shared_ptr<Task>> standby_group_task_list_;
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
  std::string desired_group_history_;
  bool is_transition_;
};
}  // namespace openrobot::ocm
