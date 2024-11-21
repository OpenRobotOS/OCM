#include "executer/executer.hpp"
#include <common/struct_type.hpp>
#include <memory>
#include "common/enum.hpp"
#include "log_anywhere/log_anywhere.hpp"
#include "node/node_map.hpp"
#include "node_test.hpp"
#include "yaml_template/task/yaml_load_generated_classes.hpp"

using namespace openrobot::ocm;

class TaskTimer : public openrobot::ocm::TaskBase {
 public:
  TaskTimer()
      : openrobot::ocm::TaskBase("openrobot_task_timer", openrobot::ocm::TimerType::INTERNAL_TIMER, 0.0),
        sem_("OPENROBOT_OCM_SEM_TIMING_GENERATOR", 0),
        shm_("OPENROBOT_OCM_SHM_TIMING_GENERATOR_DT", sizeof(uint8_t)) {
    shm_.Lock();
    *shm_.Get() = 1;  // ms
    shm_.UnLock();
  }
  void Run() override { sem_.IncrementWhenZero(); }

 private:
  openrobot::ocm::SharedMemorySemaphore sem_;
  openrobot::ocm::SharedMemoryData shm_;
};

int main() {
  openrobot::ocm::LoggerConfig log_config;
  log_config.logger_name = "my_custom_logger";
  log_config.log_file = "my_logs/application.log";
  log_config.queue_size = 16384;  // 可选，默认8192
  log_config.thread_count = 2;    // 可选，默认1
  auto& logger = openrobot::ocm::LogAnywhere::getInstance(log_config);

  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);
  timer_task.TaskStart();
  ConfigCollect& config = ConfigCollect::getInstance();
  config.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/task");
  // config.print();

  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>());
  node_map->AddNode("NodeB", std::make_shared<NodeB>());
  node_map->AddNode("NodeC", std::make_shared<NodeC>());
  node_map->AddNode("NodeD", std::make_shared<NodeD>());
  node_map->AddNode("NodeE", std::make_shared<NodeE>());
  ExecuterConfig executer_config;

  const auto& executer_setting = config.get_task_config().ExecuterSetting();
  const auto& task_list = config.get_task_config().TaskList();
  const auto& exclusive_task_group = config.get_task_config().ExclusiveTaskGroup();

  executer_config.executer_setting.package_name = "executer_test";
  executer_config.executer_setting.sem_name = "OPENROBOT_OCM_SEM_TIMING_GENERATOR";
  executer_config.executer_setting.timer_setting.timer_type = timer_type_map.at(executer_setting.TimerSetting().TimerType());
  executer_config.executer_setting.timer_setting.period = executer_setting.TimerSetting().Period();
  executer_config.executer_setting.system_setting.priority = executer_setting.SystemSetting().Priority();
  executer_config.executer_setting.system_setting.executer_cpu_affinity = executer_setting.SystemSetting().ExecuterCpuAffinity();
  executer_config.executer_setting.system_setting.idle_task_cpu_affinity = executer_setting.SystemSetting().IdleTaskCpuAffinity();

  for (const auto& task : task_list.ResidentGroup()) {
    TaskSetting task_setting;
    task_setting.task_name = task.TaskName();
    task_setting.timer_setting.timer_type = timer_type_map.at(task.TimerSetting().TimerType());
    task_setting.timer_setting.period = task.TimerSetting().Period();
    task_setting.system_setting.priority = task.SystemSetting().Priority();
    task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
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
    task_setting.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
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
  }

  Executer executer(executer_config, node_map);
  executer.Init();
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  std::cout << "executer exit" << std::endl;
  executer.TaskDestroy();
  return 0;
}
