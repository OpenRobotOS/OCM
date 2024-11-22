#include "log_anywhere/log_anywhere.hpp"
#include <spdlog/common.h>
#include <filesystem>
#include <iostream>

namespace openrobot::ocm {

// 构造函数，接受配置参数
LogAnywhere::LogAnywhere(const LoggerConfig& config) {
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
    logger_ = std::make_shared<spdlog::async_logger>("openrobot_ocm_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
                                                     spdlog::async_overflow_policy::block);

    // 设置日志级别和格式
    logger_->set_level(spdlog::level::trace);  // 设置最低日志级别
    logger_->set_pattern("[%T][%^%l%$] %v");
    logger_->flush_on(spdlog::level::info);
    // 注册到 spdlog 的全局注册表（可选）
    spdlog::register_logger(logger_);

    // 添加测试日志
    logger_->info("LogAnywhere 初始化成功。");
  } catch (const spdlog::spdlog_ex& ex) {
    // 处理初始化失败的情况
    std::cerr << "LogAnywhere initialization failed: " << ex.what() << std::endl;
  }
}

// 析构函数
LogAnywhere::~LogAnywhere() {
  // 在程序结束时，确保日志器被正确关闭
  spdlog::shutdown();
}

}  // namespace openrobot::ocm
