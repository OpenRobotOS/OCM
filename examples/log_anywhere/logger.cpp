#include "log_anywhere/log_anywhere.hpp"

using namespace openrobot::ocm;
int main() {
  // 配置日志
  LoggerConfig log_config;
  log_config.log_file = "my_logs/executer_test.log";  // 日志文件路径
  log_config.queue_size = 8192;                       // 可选，默认8192
  log_config.thread_count = 1;                        // 可选，默认1
  auto logger_generator = std::make_shared<LogAnywhere>(log_config);
  auto logger = GetLogger();

  logger->info("Logger 已初始化，日志文件：{}", log_config.log_file);
  logger->warn("这是一个警告信息");
  logger->error("发生错误，错误码：{}", 404);
  return 0;
}
