#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

void find_yaml_files(const fs::path& dir, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& file_map) {
  // 遍历文件夹中的内容
  for (const auto& entry : fs::recursive_directory_iterator(dir)) {
    if (entry.is_regular_file() && entry.path().extension() == ".yaml") {
      // 获取文件夹名称
      std::string folder_name = entry.path().parent_path().filename().string();
      // 获取文件名，不包括扩展名
      std::string file_name = entry.path().stem().string();

      // 在 map 中插入文件夹和文件信息
      file_map[folder_name][file_name] = entry.path().string();
    }
  }
}

int main() {
  std::string directory = "/home/lizhen/works/code/OpenRobot/OCM/examples/parameter/raise_a2_t2";  // 设置为你的文件夹路径
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> file_map;

  // 调用函数遍历文件夹
  find_yaml_files(directory, file_map);

  // 输出结果
  for (const auto& folder : file_map) {
    std::cout << "Folder: " << folder.first << std::endl;
    for (const auto& file : folder.second) {
      std::cout << "  File: " << file.first << ", Path: " << file.second << std::endl;
    }
  }

  return 0;
}
