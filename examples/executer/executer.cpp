#include "executer/executer.hpp"
#include <memory>
#include "common/enum.hpp"
#include "node/node_map.hpp"
#include "node_test.hpp"
#include "yaml_load_generated_classes.hpp"

using namespace openrobot::ocm;

class TaskTimer : public openrobot::ocm::TaskBase {
 public:
  TaskTimer()
      : openrobot::ocm::TaskBase("openrobot_task_timer", openrobot::ocm::TaskType::INTERNAL_TIMER, 0.0),
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
  TaskTimer timer_task;
  timer_task.SetPeriod(0.001);
  timer_task.TaskStart();
  ConfigCollect& config = ConfigCollect::getInstance();
  config.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/");
  // config.print();

  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>());
  node_map->AddNode("NodeB", std::make_shared<NodeB>());
  node_map->AddNode("NodeC", std::make_shared<NodeC>());
  node_map->AddNode("NodeD", std::make_shared<NodeD>());
  node_map->AddNode("NodeE", std::make_shared<NodeE>());
  ExecuterConfig executer_config;
  const auto& concurrent_group_config = config.get_Task_TaskSetting().ConcurrentGroup();
  const auto& exclusive_group_config = config.get_Task_TaskSetting().ExclusiveGroup();
  executer_config.package_name = "openrobot_executer";
  executer_config.sem_name = "openrobot_executer_sem";
  executer_config.period = 1;
  for (const auto& concurrent_group : concurrent_group_config) {
    GroupConfig group_config;
    group_config.group_name = concurrent_group.GroupName();
    for (const auto& task : concurrent_group.TaskList()) {
      TaskConfig task_config;
      task_config.task_name = task.TaskName();
      for (const auto& node : task.NodeList()) {
        NodeConfig node_config;
        node_config.node_name = node.NodeName();
        node_config.output_enable = node.OutputEnable();
        task_config.node_list.push_back(node_config);
      }
      task_config.task_setting.timer_type = task_type_map.at(task.TaskSetting().TimerType());
      task_config.task_setting.period = task.TaskSetting().Period();
      task_config.system_setting.priority = task.SystemSetting().Priority();
      task_config.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
      task_config.launch_setting.pre_node = task.LaunchSetting().PreNode();
      task_config.launch_setting.delay = task.LaunchSetting().Delay();
      group_config.task_list.push_back(task_config);
    }
    executer_config.concurrent_group[group_config.group_name] = group_config;
  }
  for (const auto& exclusive_group : exclusive_group_config) {
    GroupConfig group_config;
    group_config.group_name = exclusive_group.GroupName();
    for (const auto& task : exclusive_group.TaskList()) {
      TaskConfig task_config;
      task_config.task_name = task.TaskName();
      for (const auto& node : task.NodeList()) {
        NodeConfig node_config;
        node_config.node_name = node.NodeName();
        node_config.output_enable = node.OutputEnable();
        task_config.node_list.push_back(node_config);
      }
      task_config.task_setting.timer_type = task_type_map.at(task.TaskSetting().TimerType());
      task_config.task_setting.period = task.TaskSetting().Period();
      task_config.system_setting.priority = task.SystemSetting().Priority();
      task_config.system_setting.cpu_affinity = task.SystemSetting().CpuAffinity();
      task_config.launch_setting.pre_node = task.LaunchSetting().PreNode();
      task_config.launch_setting.delay = task.LaunchSetting().Delay();
      group_config.task_list.push_back(task_config);
    }
    executer_config.exclusive_group[group_config.group_name] = group_config;
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
