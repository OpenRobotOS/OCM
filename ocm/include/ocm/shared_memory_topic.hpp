// Start of Selection
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ocm/shard_memory_data.hpp"
#include "ocm/shared_memory_semaphore.hpp"

namespace openrobot::ocm {
/**
 * @brief 共享内存主题管理器。
 * @brief Shared memory topic manager.
 *
 * `SharedMemoryTopic` 类简化了使用共享内存发布和订阅主题的过程。
 * The `SharedMemoryTopic` class facilitates publishing and subscribing to topics using shared memory.
 * 它管理多个共享内存段和信号量，允许不同主题之间高效的进程间通信。
 * It manages multiple shared memory segments and semaphores, allowing efficient inter-process communication
 * for different topics.
 */
class SharedMemoryTopic {
 public:
  /**
   * @brief 默认构造函数。
   * @brief Default constructor.
   *
   * 初始化 `SharedMemoryTopic` 实例。
   * Initializes the `SharedMemoryTopic` instance.
   */
  SharedMemoryTopic() = default;

  /**
   * @brief 删除的拷贝构造函数。
   * @brief Deleted copy constructor.
   *
   * 防止复制 `SharedMemoryTopic` 实例以保持唯一所有权语义。
   * Prevents copying of `SharedMemoryTopic` instances to maintain unique ownership semantics.
   */
  SharedMemoryTopic(const SharedMemoryTopic&) = delete;

  /**
   * @brief 删除的拷贝赋值运算符。
   * @brief Deleted copy assignment operator.
   *
   * 防止将一个 `SharedMemoryTopic` 赋值给另一个以保持唯一所有权语义。
   * Prevents assignment from one `SharedMemoryTopic` to another to maintain unique ownership semantics.
   */
  SharedMemoryTopic& operator=(const SharedMemoryTopic&) = delete;

  /**
   * @brief 删除的移动构造函数。
   * @brief Deleted move constructor.
   *
   * 防止移动 `SharedMemoryTopic` 实例以保持唯一所有权语义。
   * Prevents moving of `SharedMemoryTopic` instances to maintain unique ownership semantics.
   */
  SharedMemoryTopic(SharedMemoryTopic&&) = delete;

  /**
   * @brief 删除的移动赋值运算符。
   * @brief Deleted move assignment operator.
   *
   * 防止移动赋值 `SharedMemoryTopic` 实例以保持唯一所有权语义。
   * Prevents move assignment of `SharedMemoryTopic` instances to maintain unique ownership semantics.
   */
  SharedMemoryTopic& operator=(SharedMemoryTopic&&) = delete;

  /**
   * @brief 析构函数。
   * @brief Destructor.
   *
   * 默认析构函数确保共享内存主题的正确清理。
   * Default destructor ensures proper cleanup of shared memory topics.
   */
  ~SharedMemoryTopic() = default;

  /**
   * @brief 发布单个消息到指定主题。
   * @brief Publishes a single message to a specified topic.
   *
   * 将消息写入与 `shm_name` 关联的共享内存段，并发送与 `topic_name` 关联的信号量以通知订阅者。
   * Writes the message to the shared memory segment associated with `shm_name` and signals the semaphore
   * associated with `topic_name` to notify subscribers.
   *
   * @tparam MessageType 发布消息的类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @tparam MessageType The type of the message to publish. Must support `encode` and `getEncodedSize` methods.
   * @param topic_name 发布到的主题名。
   * @param topic_name The name of the topic to publish to.
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param msg 指向要发布的消息的指针。
   * @param msg Pointer to the message to be published.
   *
   * @throws std::runtime_error 如果写入共享内存或发布信号量失败。
   * @throws std::runtime_error If writing to shared memory or publishing the semaphore fails.
   */
  template <class MessageType>
  void Publish(const std::string& topic_name, const std::string& shm_name, const MessageType* msg) {
    WriteDataToSHM(shm_name, msg);
    PublishSem(topic_name);
  }

  /**
   * @brief 发布多个消息到多个指定主题。
   * @brief Publishes a list of messages to multiple specified topics.
   *
   * 将消息列表写入与 `shm_name` 关联的共享内存段，并发送与提供的每个 `topic_name` 关联的信号量以通知订阅者。
   * Writes the list of messages to the shared memory segment associated with `shm_name` and signals the semaphores
   * associated with each `topic_name` in the provided vector.
   *
   * @tparam MessageType 发布消息的类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @tparam MessageType The type of the messages to publish. Must support `encode` and `getEncodedSize` methods.
   * @param topic_names 发布消息的主题名称向量。
   * @param topic_names Vector of topic names to publish the messages to.
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param msgs 要发布的消息向量。
   * @param msgs Vector of messages to be published.
   *
   * @throws std::runtime_error 如果写入共享内存或发布任何信号量失败。
   * @throws std::runtime_error If writing to shared memory or publishing any semaphore fails.
   */
  template <class MessageType>
  void PublishList(const std::vector<std::string>& topic_names, const std::string& shm_name, const std::vector<MessageType>& msgs) {
    WriteDataToSHM(shm_name, msgs);
    for (const auto& topic : topic_names) {
      PublishSem(topic);
    }
  }

  /**
   * @brief 订阅指定主题并使用回调处理接收的消息。
   * @brief Subscribes to a specified topic and processes the received message using a callback.
   *
   * 等待与 `topic_name` 关联的信号量，读取共享内存段 `shm_name` 中的消息，
   * 解码它，并使用解码后的消息调用提供的 `callback`。
   * Waits for the semaphore associated with `topic_name`, reads the message from the shared memory segment `shm_name`,
   * decodes it, and invokes the provided `callback` with the decoded message.
   *
   * @tparam MessageType 订阅的消息类型。必须支持 `decode` 方法。
   * @tparam MessageType The type of the message to subscribe to. Must support `decode` method.
   * @tparam Callback 处理接收消息的回调函数类型。
   * @tparam Callback The type of the callback function to process the received message.
   * @param topic_name 要订阅的主题名。
   * @param topic_name The name of the topic to subscribe to.
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param callback 处理接收消息的回调函数。
   * @param callback The callback function to process the received message.
   *
   * @throws std::runtime_error 如果访问共享内存或信号量失败。
   * @throws std::runtime_error If accessing shared memory or semaphore fails.
   */
  template <class MessageType, typename Callback>
  void Subscribe(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->Decrement();
    CheckSHMExist(shm_name, false);
    MessageType msg;
    shm_map_.at(shm_name)->Lock();
    msg.decode(shm_map_.at(shm_name)->Get(), 0, shm_map_.at(shm_name)->GetSize());
    shm_map_.at(shm_name)->UnLock();
    callback(msg);
  }

  /**
   * @brief 尝试订阅指定主题而不阻塞。
   * @brief Attempts to subscribe to a specified topic without blocking.
   *
   * 尝试减少与 `topic_name` 关联的信号量。如果成功，则从共享内存段 `shm_name` 中读取并解码消息，
   * 并使用解码后的消息调用提供的 `callback`。
   * Tries to decrement the semaphore associated with `topic_name`. If successful, reads and decodes the message
   * from the shared memory segment `shm_name` and invokes the provided `callback` with the decoded message.
   *
   * @tparam MessageType 订阅的消息类型。必须支持 `decode` 方法。
   * @tparam MessageType The type of the message to subscribe to. Must support `decode` method.
   * @tparam Callback 处理接收消息的回调函数类型。
   * @tparam Callback The type of the callback function to process the received message.
   * @param topic_name 要订阅的主题名。
   * @param topic_name The name of the topic to subscribe to.
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param callback 处理接收消息的回调函数。
   * @param callback The callback function to process the received message.
   */
  template <class MessageType, typename Callback>
  void SubscribeNoWait(const std::string& topic_name, const std::string& shm_name, Callback callback) {
    CheckSemExist(topic_name);
    if (sem_map_.at(topic_name)->TryDecrement()) {
      CheckSHMExist(shm_name, false);
      MessageType msg;
      shm_map_.at(shm_name)->Lock();
      msg.decode(shm_map_.at(shm_name)->Get(), 0, shm_map_.at(shm_name)->GetSize());
      shm_map_.at(shm_name)->UnLock();
      callback(msg);
    }
  }

 private:
  /**
   * @brief 将消息写入共享内存段。
   * @brief Writes a message to the shared memory segment.
   *
   * 将 `msg` 编码到由 `shm_name` 标识的共享内存段中。
   * Encodes the `msg` into the shared memory segment identified by `shm_name`.
   *
   * @tparam MessageType 要写入的消息类型。必须支持 `encode` 和 `getEncodedSize` 方法。
   * @tparam MessageType The type of the message to write. Must support `encode` and `getEncodedSize` methods.
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param msg 指向要写入的消息的指针。
   * @param msg Pointer to the message to be written.
   *
   * @throws std::runtime_error 如果写入共享内存失败。
   * @throws std::runtime_error If writing to shared memory fails.
   */
  template <class MessageType>
  void WriteDataToSHM(const std::string& shm_name, const MessageType* msg) {
    int datalen = msg->getEncodedSize();
    CheckSHMExist(shm_name, true, datalen);
    shm_map_.at(shm_name)->Lock();
    msg->encode(shm_map_.at(shm_name)->Get(), 0, datalen);
    shm_map_.at(shm_name)->UnLock();
  }

  /**
   * @brief 信号量通知主题。
   * @brief Signals the semaphore associated with a topic.
   *
   * 如果指定的 `topic_name` 当前值为零，则增加其信号量。
   * Increments the semaphore for the specified `topic_name` if its current value is zero.
   *
   * @param topic_name 要通知的主题名。
   * @param topic_name The name of the topic whose semaphore is to be signaled.
   *
   * @throws std::runtime_error 如果信号量通知失败。
   * @throws std::runtime_error If signaling the semaphore fails.
   */
  void PublishSem(const std::string& topic_name) {
    CheckSemExist(topic_name);
    sem_map_.at(topic_name)->IncrementWhenZero();
  }

  /**
   * @brief 确保共享内存段存在。
   * @brief Ensures that the shared memory segment exists.
   *
   * 如果由 `shm_name` 标识的共享内存段不存在，则创建一个新的。
   * 如果 `check_size` 为真，则检查大小是否符合预期。
   * If the shared memory segment identified by `shm_name` does not exist, it creates a new one.
   * Optionally checks if the size matches the expected size.
   *
   * @param shm_name 共享内存段的名称。
   * @param shm_name The name of the shared memory segment.
   * @param check_size 是否检查共享内存段的大小。
   * @param check_size Flag indicating whether to verify the size of the shared memory segment.
   * @param size 共享内存段的预期大小（以字节为单位）。如果 `check_size` 为真，则需要提供。
   * @param size The expected size of the shared memory segment in bytes. Required if `check_size` is true.
   *
   * @throws std::runtime_error 如果创建或访问共享内存失败。
   * @throws std::runtime_error If creating or accessing shared memory fails.
   */
  void CheckSHMExist(const std::string& shm_name, bool check_size, int size = 0) {
    if (shm_map_.find(shm_name) == shm_map_.end()) {
      shm_map_.emplace(shm_name, std::make_shared<SharedMemoryData<uint8_t>>(shm_name, check_size, size));
    }
  }

  /**
   * @brief 确保主题的信号量存在。
   * @brief Ensures that the semaphore for a topic exists.
   *
   * 如果与 `topic_name` 关联的信号量不存在，则创建一个新的。
   * If the semaphore associated with `topic_name` does not exist, it creates a new one.
   *
   * @param topic_name 要确保的主题名。
   * @param topic_name The name of the topic whose semaphore is to be ensured.
   *
   * @throws std::runtime_error 如果创建或访问信号量失败。
   * @throws std::runtime_error If creating or accessing the semaphore fails.
   */
  void CheckSemExist(const std::string& topic_name) {
    if (sem_map_.find(topic_name) == sem_map_.end()) {
      sem_map_.emplace(topic_name, std::make_shared<SharedMemorySemaphore>(topic_name, 0));
    }
  }

  std::unordered_map<std::string, std::shared_ptr<SharedMemoryData<uint8_t>>>
      shm_map_; /**< 共享内存段的名称键映射。<br>Map of shared memory segments keyed by their names */
  std::unordered_map<std::string, std::shared_ptr<SharedMemorySemaphore>>
      sem_map_; /**< 主题名称键的信号量映射。<br>Map of semaphores keyed by topic names */
};

}  // namespace openrobot::ocm
// End of Selection
