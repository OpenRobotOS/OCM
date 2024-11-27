#include "debug_anywhere/debug_anywhere.hpp"
#include "common/ip_tool.hpp"
#include "debug_anywhere/DebugData.hpp"
#include "lcm/lcm-cpp.hpp"
#include "task/rt/sched_rt.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a DebugDataStruct with the given topic and data.
 * @brief 使用给定的主题和数据构造 DebugDataStruct。
 * @param t The topic name.
 * @param t 主题名称。
 * @param v The vector of data values.
 * @param v 数据值的向量。
 */
DebugDataStruct::DebugDataStruct(const std::string& t, const std::vector<double>& v) : topic(t), data(v) {}

/**
 * @brief Constructs a DebugAnywhereTask with the specified configuration and queue size.
 * @brief 使用指定的配置和队列大小构造 DebugAnywhereTask。
 * @param config Configuration settings for the task.
 * @param config 任务的配置设置。
 * @param queue_size Maximum size of the internal data queue.
 * @param queue_size 内部数据队列的最大大小。
 */
DebugAnywhereTask::DebugAnywhereTask(const DebugAnywhereConfig& config) : config_(config), data_queue_(config.queue_size), sem_(0), running_(false) {
  running_.store(true);
  thread_ = std::thread([this] { Loop(); });
}

/**
 * @brief Destructor that stops the task and joins the thread.
 * @brief 析构函数，停止任务并加入线程。
 */
DebugAnywhereTask::~DebugAnywhereTask() {
  running_.store(false);
  sem_.release();
  if (thread_.joinable()) {
    thread_.join();
  }
}

/**
 * @brief The main loop that processes and publishes debug data.
 * @brief 处理并发布调试数据的主循环。
 */
void DebugAnywhereTask::Loop() {
  rt::set_thread_name("debug_anywhere");
  pid_t pid = gettid();
  if (config_.all_priority_enable) {
    rt::set_thread_priority(pid, config_.system_setting.priority, SCHED_FIFO);
  }
  if (config_.all_cpu_affinity_enable) {
    rt::set_thread_cpu_affinity(pid, config_.system_setting.cpu_affinity);
  }
  auto lcm = std::make_shared<lcm::LCM>("udpm://239.255.76.67:" + config_.port + "?ttl=" + config_.ttl);
  ip_hash_ = ipv4_to_unique_hash_hex(config_.ip);

  while (running_.load()) {
    DebugDataStruct* data_ptr;
    while (data_queue_.pop(data_ptr)) {
      DebugData debug_data;
      debug_data.count = data_ptr->data.size();
      debug_data.data = data_ptr->data;
      lcm->publish(data_ptr->topic + "_" + ip_hash_, &debug_data);
      delete data_ptr;
    }
    sem_.acquire();
  }
}

/**
 * @brief Publishes debug data to a specified channel.
 * @brief 将调试数据发布到指定的频道。
 * @param channel The name of the channel to publish to.
 * @param channel 要发布到的频道名称。
 * @param data The vector of data values to publish.
 * @param data 要发布的数据值的向量。
 */
void DebugAnywhereTask::Publish(const std::string& channel, const std::vector<double>& data) {
  if (!config_.enable) {
    return;
  }
  if (config_.white_list_enable && std::find(config_.white_list.begin(), config_.white_list.end(), channel) == config_.white_list.end()) {
    return;
  }
  DebugDataStruct* data_ptr = new DebugDataStruct(channel, data);
  if (data_queue_.push(data_ptr)) {
    sem_.release();
  } else {
    delete data_ptr;
  }
}

/**
 * @brief Retrieves the singleton instance of DebugAnywhere.
 * @brief 获取 DebugAnywhere 的单例实例。
 * @return Reference to the singleton instance.
 * @return 单例实例的引用。
 */
DebugAnywhere& DebugAnywhere::getInstance() {
  static DebugAnywhere instance;
  return instance;
}

/**
 * @brief Initializes the DebugAnywhere singleton with the given configuration and queue size.
 * @brief 使用给定的配置和队列大小初始化 DebugAnywhere 单例。
 * @param config Configuration settings for the debug task.
 * @param config 调试任务的配置设置。
 * @param queue_size Maximum size of the internal data queue.
 * @param queue_size 内部数据队列的最大大小。
 */
void DebugAnywhere::initialize(const DebugAnywhereConfig& config) { getInstance().task_ = std::make_shared<DebugAnywhereTask>(config); }

/**
 * @brief Publishes debug data to a specified channel.
 * @brief 将调试数据发布到指定的频道。
 * @param channel The name of the channel to publish to.
 * @param channel 要发布到的频道名称。
 * @param data The vector of data values to publish.
 * @param data 要发布的数据值的向量。
 */
void DebugAnywhere::Publish(const std::string& channel, const std::vector<double>& data) { getInstance().task_->Publish(channel, data); }

/**
 * @brief Private constructor to prevent direct instantiation.
 * @brief 私有构造函数，防止直接实例化。
 */
DebugAnywhere::DebugAnywhere() = default;

/**
 * @brief Destructor.
 * @brief 析构函数。
 */
DebugAnywhere::~DebugAnywhere() = default;

}  // namespace openrobot::ocm
