// debug_anywhere.cpp

#include "debug_anywhere/debug_anywhere.hpp"

namespace openrobot::ocm {

/**
 * @brief Constructs a DebugDataStruct with the given topic and data.
 * @param t The topic name.
 * @param v The vector of data values.
 */
DebugDataStruct::DebugDataStruct(const std::string& t, const std::vector<double>& v) : topic(t), data(v) {}

/**
 * @brief Constructs a DebugAnywhereTask with the specified configuration and queue size.
 * @param config Configuration settings for the task.
 * @param queue_size Maximum size of the internal data queue.
 */
DebugAnywhereTask::DebugAnywhereTask(const DebugAnywhereConfig& config, const size_t queue_size)
    : config_(config), data_queue_(queue_size), sem_(0), running_(false) {
  running_.store(true);
  thread_ = std::thread([this] { Loop(); });
}

/**
 * @brief Destructor that stops the task and joins the thread.
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
 * @param channel The name of the channel to publish to.
 * @param data The vector of data values to publish.
 */
void DebugAnywhereTask::Publish(const std::string& channel, const std::vector<double>& data) {
  if (!config_.enable) {
    return;
  }
  if (config_.white_list_enable && config_.white_list.find(channel) == config_.white_list.end()) {
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
 * @return Reference to the singleton instance.
 */
DebugAnywhere& DebugAnywhere::getInstance() {
  static DebugAnywhere instance;
  return instance;
}

/**
 * @brief Initializes the DebugAnywhere singleton with the given configuration and queue size.
 * @param config Configuration settings for the debug task.
 * @param queue_size Maximum size of the internal data queue.
 */
void DebugAnywhere::initialize(const DebugAnywhereConfig& config, const size_t queue_size) {
  getInstance().task_ = std::make_shared<DebugAnywhereTask>(config, queue_size);
}

/**
 * @brief Publishes debug data to a specified channel.
 * @param channel The name of the channel to publish to.
 * @param data The vector of data values to publish.
 */
void DebugAnywhere::Publish(const std::string& channel, const std::vector<double>& data) { getInstance().task_->Publish(channel, data); }

/**
 * @brief Private constructor to prevent direct instantiation.
 */
DebugAnywhere::DebugAnywhere() = default;

/**
 * @brief Destructor.
 */
DebugAnywhere::~DebugAnywhere() = default;

}  // namespace openrobot::ocm
