#include "executer/executer.hpp"
#include <common/struct_type.hpp>
#include <memory>
#include <ocm/ocm.hpp>
#include "common/enum.hpp"
#include "executer/desired_group_data.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "node/node_map.hpp"
#include "node_test.hpp"
#include "yaml_template/task/yaml_load_generated_classes.hpp"

using namespace openrobot::ocm;

class TaskTimer : public openrobot::ocm::TaskBase {
 public:
  TaskTimer() : openrobot::ocm::TaskBase("openrobot_task_timer", openrobot::ocm::TimerType::INTERNAL_TIMER, 0.0, false, false) {
    std::vector<std::string> sem_name_list = {"executer", "resident_task_1", "standby_task_1", "standby_task_2", "standby_task_3"};
    for (const auto& sem_name : sem_name_list) {
      sem_.emplace_back(SharedMemorySemaphore(sem_name, 0));
      shm_.emplace_back(SharedMemoryData<uint8_t>(sem_name, true, sizeof(uint8_t)));
    }
    for (auto& shm : shm_) {
      shm.Lock();
      *shm.Get() = 1;  // ms
      shm.UnLock();
    }
  }
  ~TaskTimer() = default;
  void Run() override {
    for (auto& sem : sem_) {
      sem.IncrementWhenZero();
    }
  }

 private:
  std::vector<openrobot::ocm::SharedMemorySemaphore> sem_;
  std::vector<openrobot::ocm::SharedMemoryData<uint8_t>> shm_;
};

int main() {
  openrobot::ocm::LoggerConfig log_config;
  log_config.log_file = "my_logs/executer_test.log";
  log_config.queue_size = 8192;  // 可选，默认8192
  log_config.thread_count = 1;   // 可选，默认1
  auto logger_generator = std::make_shared<openrobot::ocm::LogAnywhere>(log_config);
  auto logger = GetLogger();

  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);
  SystemSetting timer_system_setting;
  timer_system_setting.priority = 0;
  timer_system_setting.cpu_affinity = {0};
  timer_task.TaskStart(timer_system_setting);

  ConfigCollect& config = ConfigCollect::getInstance();
  config.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/task");

  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>("NodeA"));
  node_map->AddNode("NodeB", std::make_shared<NodeB>("NodeB"));
  node_map->AddNode("NodeC", std::make_shared<NodeC>("NodeC"));
  node_map->AddNode("NodeD", std::make_shared<NodeD>("NodeD"));
  node_map->AddNode("NodeE", std::make_shared<NodeE>("NodeE"));
  ExecuterConfig executer_config;

  const auto& executer_setting = config.get_task_config().ExecuterSetting();
  const auto& task_list = config.get_task_config().TaskList();
  const auto& exclusive_task_group = config.get_task_config().ExclusiveTaskGroup();

  executer_config.executer_setting.package_name = "executer";
  executer_config.executer_setting.timer_setting.timer_type = timer_type_map.at(executer_setting.TimerSetting().TimerType());
  executer_config.executer_setting.timer_setting.period = executer_setting.TimerSetting().Period();
  executer_config.executer_setting.system_setting.priority = static_cast<int>(executer_setting.SystemSetting().Priority());
  // executer_config.executer_setting.system_setting.cpu_affinity = executer_setting.SystemSetting().ExecuterCpuAffinity();

  for (const auto& task : task_list.ResidentGroup()) {
    TaskSetting task_setting;
    task_setting.task_name = task.TaskName();
    task_setting.timer_setting.timer_type = timer_type_map.at(task.TimerSetting().TimerType());
    task_setting.timer_setting.period = task.TimerSetting().Period();
    task_setting.system_setting.priority = task.SystemSetting().Priority();
    // task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
    task_setting.launch_setting.pre_node = task.LaunchSetting().PreNode();
    task_setting.launch_setting.delay = task.LaunchSetting().Delay();
    for (const auto& node : task.NodeList()) {
      NodeConfig node_config;
      node_config.node_name = node.NodeName();
      node_config.output_enable = node.OutputEnable();
      task_setting.node_list.push_back(node_config);
    }
    executer_config.task_list.resident_group[task.TaskName()] = task_setting;
  }

  for (const auto& task : task_list.StandbyGroup()) {
    TaskSetting task_setting;
    task_setting.task_name = task.TaskName();
    task_setting.timer_setting.timer_type = timer_type_map.at(task.TimerSetting().TimerType());
    task_setting.timer_setting.period = task.TimerSetting().Period();
    task_setting.system_setting.priority = task.SystemSetting().Priority();
    // task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
    for (const auto& node : task.NodeList()) {
      NodeConfig node_config;
      node_config.node_name = node.NodeName();
      node_config.output_enable = node.OutputEnable();
      task_setting.node_list.push_back(node_config);
    }
    executer_config.task_list.standby_group[task.TaskName()] = task_setting;
  }

  for (const auto& group : exclusive_task_group) {
    GroupSetting group_setting;
    group_setting.group_name = group.GroupName();
    for (const auto& task : group.TaskList()) {
      GroupTaskSetting group_task_setting;
      group_task_setting.task_name = task.TaskName();
      group_task_setting.force_init_node = task.ForceInitNode();
      group_task_setting.pre_node = task.PreNode();
      group_setting.task_list[task.TaskName()] = group_task_setting;
    }
    executer_config.exclusive_task_group[group.GroupName()] = group_setting;
  }

  Executer executer(executer_config, node_map);
  executer.CreateTask();
  executer.InitTask();
  SharedMemoryTopic desired_group_topic;
  DesiredGroupData desired_group_data;
  desired_group_data.desired_group = "passive";
  desired_group_topic.Publish("executer_desired_group", "executer_desired_group", &desired_group_data);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  desired_group_data.desired_group = "pdstand";
  desired_group_topic.Publish("executer_desired_group", "executer_desired_group", &desired_group_data);
  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "executer exit" << std::endl;
  timer_task.TaskDestroy();
  std::cout << "timer_task destroyed" << std::endl;
  executer.ExitAllTask();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  executer.TaskDestroy();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return 0;
}
