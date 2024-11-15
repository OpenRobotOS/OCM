#include "logger/logger.hpp"
#include <spdlog/common.h>

int main() {
  // 定义日志配置
  openrobot::ocm::LoggerConfig config;
  config.logger_name = "my_custom_logger";
  config.log_file = "my_logs/application.log";
  config.queue_size = 16384;  // 可选，默认8192
  config.thread_count = 2;    // 可选，默认1
  auto& logger = openrobot::ocm::Logger::getInstance(config);
  logger.setLogLevel(spdlog::level::critical);
  // 初始化 Logger 单例
  logger.info("Logger 已初始化，日志文件：{}", config.log_file);
  logger.warn("这是一个警告信息");
  logger.error("发生错误，错误码：{}", 404);
  logger.setLogLevel(spdlog::level::debug);
  // 继续记录日志
  logger.debug("这是一条调试信息");
  logger.critical("这是一个严重错误信息");

  // 动态调整日志级别

  logger.trace("这条跟踪信息将不会被记录，因为当前日志级别是 debug");
  logger.debug("这条调试信息将被记录");

  return 0;
}
