// debug_anywhere.hpp
#pragma once

#include <boost/lockfree/queue.hpp>
#include <memory>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include "DebugData.hpp"
#include "common/ip_tool.hpp"
#include "lcm/lcm-cpp.hpp"
#include "ocm/ocm.hpp"
#include "task/task_base.hpp"

namespace openrobot::ocm {
struct DebugAnywhereConfig {
  std::string ip;
  std::string port;
  std::string ttl;
};
struct DebugDataStruct {
  std::string topic;
  std::vector<double> data;

  DebugDataStruct(const std::string& t, const std::vector<double>& v) : topic(t), data(v) {}
};

class DebugAnywhereTask {
 public:
  DebugAnywhereTask(const DebugAnywhereConfig& config) : config_(config), data_queue_(0), sem_(0) {
    thread_ = std::thread([this] { Loop(); });
  }
  ~DebugAnywhereTask() = default;

  void Loop() {
    auto lcm = std::make_shared<lcm::LCM>("udpm://239.255.76.67:" + config_.port + "?ttl=" + config_.ttl);
    ip_hash_ = ipv4_to_unique_hash_hex(config_.ip);
    while (true) {
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

  void Publish(const std::string& channel, const std::vector<double>& data) {
    DebugDataStruct* data_ptr = new DebugDataStruct(channel, data);
    if (data_queue_.push(data_ptr)) {
      sem_.release();
    } else {
      delete data_ptr;
    }
  }

 private:
  std::thread thread_;
  std::string ip_hash_;
  DebugAnywhereConfig config_;
  boost::lockfree::queue<DebugDataStruct*> data_queue_;
  std::binary_semaphore sem_;
};

class DebugAnywhere {
 public:
  // Delete copy constructor and assignment operator
  DebugAnywhere(const DebugAnywhere&) = delete;
  DebugAnywhere& operator=(const DebugAnywhere&) = delete;

  static DebugAnywhere& getInstance() {
    static DebugAnywhere instance;  // 无参数构造
    return instance;
  }

  static void initialize(const DebugAnywhereConfig& config) { getInstance().task_ = std::make_shared<DebugAnywhereTask>(config); }

  void Publish(const std::string& channel, const std::vector<double>& data) { getInstance().task_->Publish(channel, data); }

 private:
  // Private constructor to prevent direct instantiation
  DebugAnywhere() = default;
  ~DebugAnywhere() = default;
  std::shared_ptr<DebugAnywhereTask> task_;
};
}  // namespace openrobot::ocm