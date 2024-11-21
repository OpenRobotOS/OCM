#include <iostream>
#include <string>
#include "generated_classes.hpp"  // Assuming ConfigCollect is in this header

int main() {
  // Define the base path where your YAML files are located
  std::string base_path = "/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template";  // Change this to the actual directory

  // Instantiate the ConfigCollect object
  openrobot::ocm::ConfigCollect& config = openrobot::ocm::ConfigCollect::getInstance();
  config.update_from_yaml_all("/home/lizhen/works/code/OpenRobot/OCM/examples/executer/yaml_template/");

  config.print();

  return 0;
}
