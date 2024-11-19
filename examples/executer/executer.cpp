#include "executer/executer.hpp"
#include <memory>
#include "generated_classes.hpp"
#include "node/node_map.hpp"
#include "node_test.hpp"

using namespace openrobot::ocm;

int main() {
  ConfigCollect config_collector;
  config_collector.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/");
  config_collector.print();

  std::shared_ptr<NodeMap> node_map = std::make_shared<NodeMap>();
  node_map->AddNode("NodeA", std::make_shared<NodeA>());
  node_map->AddNode("NodeB", std::make_shared<NodeB>());
  node_map->AddNode("NodeC", std::make_shared<NodeC>());
  node_map->AddNode("NodeD", std::make_shared<NodeD>());
  node_map->AddNode("NodeE", std::make_shared<NodeE>());
  ExecuterConfig executer_config;
  executer_config.package_name = "openrobot";
  executer_config.sem_name = "openrobot_executer_sem";
  executer_config.period = 1;
  GroupConfig group_config;
  group_config.group_name = "group1";
  TaskConfig task_config;
  task_config.task_name = "NodeA";
  NodeConfig node_config;
  node_config.node_name = "NodeA";
  node_config.output_enable = true;
  task_config.node_list.push_back(node_config);
  task_config.task_setting.timer_type = TaskType::INTERNAL_TIMER;
  task_config.task_setting.period = 1;
  group_config.task_list.push_back(task_config);
  executer_config.concurrent_group[group_config.group_name] = group_config;
  Executer executer(executer_config, node_map);
  executer.Init();

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  executer.TaskDestroy();
  return 0;
}
