#pragma once

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <memory>
#include <set>
namespace openrobot::ocm {

inline std::string JointStrSet(const std::set<std::string>& s, const std::string& delimiter) {
  std::string result;
  for (auto it = s.begin(); it != s.end(); ++it) {
    if (it != s.begin()) {
      result += delimiter;
    }
    result += *it;
  }
  return result;
}
enum class ColorEnum : uint8_t { RED = 0, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

inline std::string ColorPrint(const std::string& str, ColorEnum color) {
  std::string color_code;

  // 根据 ColorEnum 枚举值设置 ANSI 颜色代码
  switch (color) {
    case ColorEnum::RED:
      color_code = "\033[31m";  // 红色
      break;
    case ColorEnum::GREEN:
      color_code = "\033[32m";  // 绿色
      break;
    case ColorEnum::YELLOW:
      color_code = "\033[33m";  // 黄色
      break;
    case ColorEnum::BLUE:
      color_code = "\033[34m";  // 蓝色
      break;
    case ColorEnum::MAGENTA:
      color_code = "\033[35m";  // 品红色
      break;
    case ColorEnum::CYAN:
      color_code = "\033[36m";  // 青色
      break;
    case ColorEnum::WHITE:
      color_code = "\033[37m";  // 白色
      break;
  }

  // 重置颜色
  std::string reset_code = "\033[0m";

  // 返回带颜色的字符串
  return color_code + str + reset_code;
}
// 配置结构体
struct LoggerConfig {
  std::string log_file = "logs/log.txt";  // 日志文件路径
  size_t queue_size = 8192;               // 队列大小
  size_t thread_count = 1;                // 后台线程数
};

// LogAnywhere 单例类
class LogAnywhere {
 public:
  LogAnywhere(const LoggerConfig& config);
  ~LogAnywhere();

  // 删除拷贝构造函数和赋值运算符，防止拷贝
  LogAnywhere(const LogAnywhere&) = delete;
  LogAnywhere& operator=(const LogAnywhere&) = delete;

  std::shared_ptr<spdlog::logger> logger_;  // spdlog 日志器
};
}  // namespace openrobot::ocm
