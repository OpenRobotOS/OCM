#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <unordered_map>

namespace openrobot::ocm {
class ParameterHandler {
 public:
  // 删除拷贝构造函数和赋值运算符
  ParameterHandler(const ParameterHandler&) = delete;
  ParameterHandler& operator=(const ParameterHandler&) = delete;

  // 提供一个全局访问点
  static ParameterHandler& getInstance() {
    static ParameterHandler instance;
    return instance;
  }

  void doSomething() {}

 private:
  // 私有构造函数，防止外部实例化
  ParameterHandler() {}

  ~ParameterHandler() {}

  void find_yaml_files(const std::filesystem::path& dir, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& file_map) {
    // 遍历文件夹中的内容
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
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
};
}  // namespace openrobot::ocm