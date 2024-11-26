

#pragma once

#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <semaphore>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "common/struct_type.hpp"

namespace openrobot::ocm {

/**
 * @brief Configuration settings for DebugAnywhereTask.
 * @brief DebugAnywhereTask的配置设置。
 */
struct DebugAnywhereConfig {
  /** @brief IP address for the debug interface. */
  /** @brief 调试接口的IP地址。 */
  std::string ip;

  /** @brief Port number for the debug interface. */
  /** @brief 调试接口的端口号。 */
  std::string port;

  /** @brief Time-to-live (TTL) value for multicast packets. */
  /** @brief 多播数据包的生存时间（TTL）值。 */
  std::string ttl;

  /** @brief Flag to enable or disable the debug functionality. */
  /** @brief 启用或禁用调试功能的标志。 */
  bool enable;

  /** @brief Flag to enable all priority settings. */
  /** @brief 启用所有优先级设置的标志。 */
  bool all_priority_enable;

  /** @brief Flag to enable all CPU affinity settings. */
  /** @brief 启用所有CPU亲和性设置的标志。 */
  bool all_cpu_affinity_enable;

  /** @brief System settings related to real-time scheduling. */
  /** @brief 与实时调度相关的系统设置。 */
  SystemSetting system_setting;

  /** @brief Flag to enable the white list filtering. */
  /** @brief 启用白名单过滤的标志。 */
  bool white_list_enable;

  /** @brief Set of channels that are allowed when white list is enabled. */
  /** @brief 启用白名单时允许的频道集合。 */
  std::set<std::string> white_list;
};

/**
 * @brief Structure to hold debug data associated with a specific topic.
 * @brief 用于存储与特定主题相关的调试数据的结构体。
 */
struct DebugDataStruct {
  /** @brief The topic name for the debug data. */
  /** @brief 调试数据的主题名称。 */
  std::string topic;

  /** @brief Vector of double values representing the debug data. */
  /** @brief 表示调试数据的双精度值向量。 */
  std::vector<double> data;

  /**
   * @brief Constructs a DebugDataStruct with the given topic and data.
   * @brief 使用给定的主题和数据构造一个DebugDataStruct。
   * @param t The topic name.
   * @param t 主题名称。
   * @param v The vector of data values.
   * @param v 数据值的向量。
   */
  DebugDataStruct(const std::string& t, const std::vector<double>& v);
};

/**
 * @brief Task responsible for handling debug data publishing.
 * @brief 负责处理调试数据发布的任务。
 */
class DebugAnywhereTask {
 public:
  /**
   * @brief Constructs a DebugAnywhereTask with the specified configuration and queue size.
   * @brief 使用指定的配置和队列大小构造一个DebugAnywhereTask。
   * @param config Configuration settings for the task.
   * @param config 任务的配置设置。
   * @param queue_size Maximum size of the internal data queue.
   * @param queue_size 内部数据队列的最大大小。
   */
  DebugAnywhereTask(const DebugAnywhereConfig& config, const size_t queue_size);

  /**
   * @brief Destructor that stops the task and joins the thread.
   * @brief 停止任务并加入线程的析构函数。
   */
  ~DebugAnywhereTask();

  /**
   * @brief The main loop that processes and publishes debug data.
   * @brief 处理和发布调试数据的主循环。
   */
  void Loop();

  /**
   * @brief Publishes debug data to a specified channel.
   * @brief 将调试数据发布到指定的频道。
   * @param channel The name of the channel to publish to.
   * @param channel 要发布到的频道名称。
   * @param data The vector of data values to publish.
   * @param data 要发布的数据值的向量。
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /** @brief Thread running the main loop. */
  /** @brief 运行主循环的线程。 */
  std::thread thread_;

  /** @brief Hashed representation of the IP address. */
  /** @brief IP地址的哈希表示。 */
  std::string ip_hash_;

  /** @brief Configuration settings for the task. */
  /** @brief 任务的配置设置。 */
  DebugAnywhereConfig config_;

  /** @brief Lock-free queue to store incoming debug data. */
  /** @brief 用于存储传入调试数据的无锁队列。 */
  boost::lockfree::queue<DebugDataStruct*> data_queue_;

  /** @brief Semaphore to signal the availability of data. */
  /** @brief 用于指示数据可用性的信号量。 */
  std::binary_semaphore sem_;

  /** @brief Atomic flag indicating whether the task is running. */
  /** @brief 原子标志，指示任务是否正在运行。 */
  std::atomic<bool> running_;
};

/**
 * @brief Singleton class providing an interface to publish debug data anywhere.
 * @brief 提供接口以在任何地方发布调试数据的单例类。
 */
class DebugAnywhere {
 public:
  // Delete copy constructor and assignment operator
  // 删除拷贝构造函数和赋值运算符
  DebugAnywhere(const DebugAnywhere&) = delete;
  DebugAnywhere& operator=(const DebugAnywhere&) = delete;

  /**
   * @brief Retrieves the singleton instance of DebugAnywhere.
   * @brief 获取DebugAnywhere的单例实例。
   * @return Reference to the singleton instance.
   * @return 单例实例的引用。
   */
  static DebugAnywhere& getInstance();

  /**
   * @brief Initializes the DebugAnywhere singleton with the given configuration and queue size.
   * @brief 使用给定的配置和队列大小初始化DebugAnywhere单例。
   * @param config Configuration settings for the debug task.
   * @param config 调试任务的配置设置。
   * @param queue_size Maximum size of the internal data queue.
   * @param queue_size 内部数据队列的最大大小。
   */
  static void initialize(const DebugAnywhereConfig& config, const size_t queue_size);

  /**
   * @brief Publishes debug data to a specified channel.
   * @brief 将调试数据发布到指定的频道。
   * @param channel The name of the channel to publish to.
   * @param channel 要发布到的频道名称。
   * @param data The vector of data values to publish.
   * @param data 要发布的数据值的向量。
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /**
   * @brief Private constructor to prevent direct instantiation.
   * @brief 私有构造函数，以防止直接实例化。
   */
  DebugAnywhere();

  /**
   * @brief Destructor.
   * @brief 析构函数。
   */
  ~DebugAnywhere();

  /** @brief Shared pointer to the DebugAnywhereTask instance. */
  /** @brief 指向DebugAnywhereTask实例的共享指针。 */
  std::shared_ptr<DebugAnywhereTask> task_;
};

}  // namespace openrobot::ocm
