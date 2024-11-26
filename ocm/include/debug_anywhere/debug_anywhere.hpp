// debug_anywhere.hpp
#pragma once

#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <semaphore>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include "DebugData.hpp"
#include "common/ip_tool.hpp"
#include "common/struct_type.hpp"
#include "lcm/lcm-cpp.hpp"
#include "task/rt/sched_rt.hpp"

namespace openrobot::ocm {

/**
 * @brief Configuration settings for DebugAnywhereTask.
 */
struct DebugAnywhereConfig {
  /** @brief IP address for the debug interface. */
  std::string ip;

  /** @brief Port number for the debug interface. */
  std::string port;

  /** @brief Time-to-live (TTL) value for multicast packets. */
  std::string ttl;

  /** @brief Flag to enable or disable the debug functionality. */
  bool enable;

  /** @brief Flag to enable all priority settings. */
  bool all_priority_enable;

  /** @brief Flag to enable all CPU affinity settings. */
  bool all_cpu_affinity_enable;

  /** @brief System settings related to real-time scheduling. */
  SystemSetting system_setting;

  /** @brief Flag to enable the white list filtering. */
  bool white_list_enable;

  /** @brief Set of channels that are allowed when white list is enabled. */
  std::set<std::string> white_list;
};

/**
 * @brief Structure to hold debug data associated with a specific topic.
 */
struct DebugDataStruct {
  /** @brief The topic name for the debug data. */
  std::string topic;

  /** @brief Vector of double values representing the debug data. */
  std::vector<double> data;

  /**
   * @brief Constructs a DebugDataStruct with the given topic and data.
   * @param t The topic name.
   * @param v The vector of data values.
   */
  DebugDataStruct(const std::string& t, const std::vector<double>& v);
};

/**
 * @brief Task responsible for handling debug data publishing.
 */
class DebugAnywhereTask {
 public:
  /**
   * @brief Constructs a DebugAnywhereTask with the specified configuration and queue size.
   * @param config Configuration settings for the task.
   * @param queue_size Maximum size of the internal data queue.
   */
  DebugAnywhereTask(const DebugAnywhereConfig& config, const size_t queue_size);

  /**
   * @brief Destructor that stops the task and joins the thread.
   */
  ~DebugAnywhereTask();

  /**
   * @brief The main loop that processes and publishes debug data.
   */
  void Loop();

  /**
   * @brief Publishes debug data to a specified channel.
   * @param channel The name of the channel to publish to.
   * @param data The vector of data values to publish.
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /** @brief Thread running the main loop. */
  std::thread thread_;

  /** @brief Hashed representation of the IP address. */
  std::string ip_hash_;

  /** @brief Configuration settings for the task. */
  DebugAnywhereConfig config_;

  /** @brief Lock-free queue to store incoming debug data. */
  boost::lockfree::queue<DebugDataStruct*> data_queue_;

  /** @brief Semaphore to signal the availability of data. */
  std::binary_semaphore sem_;

  /** @brief Atomic flag indicating whether the task is running. */
  std::atomic<bool> running_;
};

/**
 * @brief Singleton class providing an interface to publish debug data anywhere.
 */
class DebugAnywhere {
 public:
  // Delete copy constructor and assignment operator
  DebugAnywhere(const DebugAnywhere&) = delete;
  DebugAnywhere& operator=(const DebugAnywhere&) = delete;

  /**
   * @brief Retrieves the singleton instance of DebugAnywhere.
   * @return Reference to the singleton instance.
   */
  static DebugAnywhere& getInstance();

  /**
   * @brief Initializes the DebugAnywhere singleton with the given configuration and queue size.
   * @param config Configuration settings for the debug task.
   * @param queue_size Maximum size of the internal data queue.
   */
  static void initialize(const DebugAnywhereConfig& config, const size_t queue_size);

  /**
   * @brief Publishes debug data to a specified channel.
   * @param channel The name of the channel to publish to.
   * @param data The vector of data values to publish.
   */
  void Publish(const std::string& channel, const std::vector<double>& data);

 private:
  /**
   * @brief Private constructor to prevent direct instantiation.
   */
  DebugAnywhere();

  /**
   * @brief Destructor.
   */
  ~DebugAnywhere();

  /** @brief Shared pointer to the DebugAnywhereTask instance. */
  std::shared_ptr<DebugAnywhereTask> task_;
};

}  // namespace openrobot::ocm
