#include <iostream>
#include <string>
#include "generated_classes.hpp"  // Assuming ConfigCollect is in this header

int main() {
  // Define the base path where your YAML files are located
  std::string base_path = "/home/lizhen/works/code/OpenRobot/OCM/examples/parameter/raise_a2_t2";  // Change this to the actual directory

  // Instantiate the ConfigCollect object
  openrobot::ocm::ConfigCollect config_collector;

  // Update the config collector with data from YAML files
  config_collector.update_from_yaml(base_path);

  std::cout << "ActionSetting B name: " << config_collector.get_action_setting_a_().ResidentSetting()[0].Name() << std::endl;

  return 0;
}
