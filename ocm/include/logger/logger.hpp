#pragma once

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

namespace openrobot::ocm {
// 配置结构体
struct LoggerConfig {
  std::string logger_name = "async_file_logger";  // 日志器名称
  std::string log_file = "logs/log.txt";          // 日志文件路径
  size_t queue_size = 8192;                       // 队列大小
  size_t thread_count = 1;                        // 后台线程数
};

// Logger 单例类
class Logger {
 public:
  // 获取单例实例的静态方法，接受可选的配置参数
  static Logger& getInstance(const LoggerConfig& config = LoggerConfig()) {
    // 局部静态变量，确保只初始化一次，并且线程安全
    static Logger instance(config);
    return instance;
  }

  // 删除拷贝构造函数和赋值运算符，防止拷贝
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  // 提供日志记录的方法
  template <typename... Args>
  void trace(const char* fmt, const Args&... args) {
    logger_->trace(fmt, args...);
  }

  template <typename... Args>
  void debug(const char* fmt, const Args&... args) {
    logger_->debug(fmt, args...);
  }

  template <typename... Args>
  void info(const char* fmt, const Args&... args) {
    logger_->info(fmt, args...);
  }

  template <typename... Args>
  void warn(const char* fmt, const Args&... args) {
    logger_->warn(fmt, args...);
  }

  template <typename... Args>
  void error(const char* fmt, const Args&... args) {
    logger_->error(fmt, args...);
  }

  template <typename... Args>
  void critical(const char* fmt, const Args&... args) {
    logger_->critical(fmt, args...);
  }

  // 动态设置日志级别
  void setLogLevel(spdlog::level::level_enum level) { logger_->set_level(level); }

 private:
  // 私有构造函数，接受配置参数
  Logger(const LoggerConfig& config) {
    try {
      // 确保日志目录存在
      std::filesystem::create_directories(std::filesystem::path(config.log_file).parent_path());

      // 初始化线程池，队列大小和线程数由配置决定
      spdlog::init_thread_pool(config.queue_size, config.thread_count);

      // 创建多个 sink：文件和控制台
      std::vector<spdlog::sink_ptr> sinks;
      sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.log_file, true));
      sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

      // 创建异步日志器
      logger_ = std::make_shared<spdlog::async_logger>(config.logger_name, sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                       spdlog::async_overflow_policy::block);

      // 设置日志级别和格式
      logger_->set_level(spdlog::level::trace);  // 设置最低日志级别
      logger_->set_pattern("[%P][%T][%Y-%m-%d %H:%M:%S][%^%l%$] %v");

      // 注册到 spdlog 的全局注册表（可选）
      spdlog::register_logger(logger_);
    } catch (const spdlog::spdlog_ex& ex) {
      // 处理初始化失败的情况
      std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
  }

  ~Logger() {
    // 在程序结束时，确保日志器被正确关闭
    spdlog::shutdown();
  }

  std::shared_ptr<spdlog::logger> logger_;  // spdlog 日志器
};
}  // namespace openrobot::ocm