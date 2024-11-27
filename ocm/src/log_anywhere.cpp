
#include "log_anywhere/log_anywhere.hpp"

#include <spdlog/common.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include "task/rt/sched_rt.hpp"

namespace openrobot::ocm {

// Constructor Implementation
/**
 * @brief Constructs a LogAnywhere instance with the specified configuration.
 * @brief 使用指定配置构造一个 LogAnywhere 实例。
 *
 * This constructor initializes the spdlog asynchronous logger with both file and console sinks.
 * 此构造函数使用文件和控制台接收器初始化 spdlog 异步记录器。
 * It sets the logging level to trace, defines the log message pattern, and ensures that the
 * log directory exists. It also registers the logger globally for easy access.
 * 它将日志级别设置为 trace，定义日志消息模式，并确保日志目录存在。它还全局注册记录器以便轻松访问。
 *
 * @param config Configuration settings for the logger, including log file path, queue size, and thread count.
 * @param config 记录器的配置设置，包括日志文件路径、队列大小和线程数量。
 *
 * @note If logger initialization fails, an error message is printed to the standard error stream.
 * @note 如果记录器初始化失败，将错误消息打印到标准错误流。
 */
LogAnywhere::LogAnywhere(const LoggerConfig& config) {
  rt::set_thread_name("log_anywhere");
  pid_t pid = gettid();
  if (config.all_priority_enable) {
    rt::set_thread_priority(pid, config.system_setting.priority, SCHED_FIFO);
  }
  if (config.all_cpu_affinity_enable) {
    rt::set_thread_cpu_affinity(pid, config.system_setting.cpu_affinity);
  }
  try {
    // Ensure that the log directory exists
    std::filesystem::create_directories(std::filesystem::path(config.log_file).parent_path());

    // Initialize the spdlog thread pool with the specified queue size and thread count
    spdlog::init_thread_pool(config.queue_size, config.thread_count);

    // Create sinks for file and console logging
    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.log_file, true));  // File sink
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());                     // Console sink

    // Create an asynchronous logger with the specified sinks and thread pool
    logger_ = std::make_shared<spdlog::async_logger>("openrobot_ocm_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                     spdlog::async_overflow_policy::block);

    // Set logging level to trace (captures all log levels)
    logger_->set_level(spdlog::level::trace);

    // Define the log message pattern: [Time][ColorLevel] Message
    logger_->set_pattern("[%T][%^%l%$] %v");

    // Flush logs on info level and above to ensure important messages are written promptly
    logger_->flush_on(spdlog::level::info);

    // Register the logger globally for easy access via spdlog::get()
    spdlog::register_logger(logger_);

    // Log a successful initialization message
    logger_->info("LogAnywhere initialized successfully.");
  } catch (const spdlog::spdlog_ex& ex) {
    // Handle initialization failure by outputting an error message to standard error
    std::cerr << "LogAnywhere initialization failed: " << ex.what() << std::endl;
  }
}

// Destructor Implementation
/**
 * @brief Destructs the LogAnywhere instance.
 * @brief 析构 LogAnywhere 实例。
 *
 * Ensures that the spdlog logger is properly shut down, flushing all pending log messages
 * and releasing resources.
 * 确保 spdlog 记录器被正确关闭，刷新所有待处理的日志消息并释放资源。
 */
LogAnywhere::~LogAnywhere() {
  // Shutdown spdlog to ensure all logs are flushed and resources are released
  spdlog::shutdown();
}

}  // namespace openrobot::ocm
