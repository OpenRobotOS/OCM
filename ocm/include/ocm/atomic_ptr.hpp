// start of selection
#pragma once
#include <memory>

namespace openrobot::ocm {

/**
 * @brief Atomic pointer wrapper for thread-safe read and write operations.
 * @brief 原子指针包装器，用于线程安全的读写操作。
 *
 * The `AtomicPtr` class provides a thread-safe way to manage a shared pointer to a constant
 * object of type `T`. It ensures that reads and writes to the pointer are atomic, preventing
 * data races in multi-threaded environments.
 *
 * `AtomicPtr` 类提供了一种线程安全的方式来管理指向类型 `T` 的常量对象的共享指针。它确保对指针的读写操作是原子的，防止多线程环境中的数据竞争。
 *
 * @tparam T The type of the object to be pointed to.
 * @tparam T 指针所指向对象的类型。
 */
template <typename T>
class AtomicPtr {
 public:
  /**
   * @brief Default constructor.
   * @brief 默认构造函数。
   *
   * Initializes the atomic pointer with a default-constructed `std::shared_ptr<const T>`.
   *
   * 使用默认构造的 `std::shared_ptr<const T>` 初始化原子指针。
   */
  AtomicPtr() { data_ptr_.store(std::make_shared<const T>()); }

  /**
   * @brief Constructs an AtomicPtr with the given data.
   * @brief 使用给定数据构造一个 AtomicPtr 实例。
   *
   * Initializes the atomic pointer with a `std::shared_ptr<const T>` pointing to a copy of `data`.
   *
   * 使用指向 `data` 复制品的 `std::shared_ptr<const T>` 初始化原子指针。
   *
   * @param data The data to be stored in the atomic pointer.
   * @param data 要存储在原子指针中的数据。
   */
  explicit AtomicPtr(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }

  /**
   * @brief Deleted copy constructor.
   * @brief 删除的拷贝构造函数。
   *
   * Prevents copying of `AtomicPtr` instances to maintain unique ownership semantics.
   *
   * 防止拷贝 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr(const AtomicPtr&) { throw std::logic_error("[AtomicPtr] Data copy construction is not allowed!"); }

  /**
   * @brief Deleted copy assignment operator.
   * @brief 删除的拷贝赋值运算符。
   *
   * Prevents assignment from one `AtomicPtr` to another to maintain unique ownership semantics.
   *
   * 防止将一个 `AtomicPtr` 赋值给另一个以保持唯一所有权语义。
   */
  AtomicPtr& operator=(const AtomicPtr&) = delete;

  /**
   * @brief Deleted move constructor.
   * @brief 删除的移动构造函数。
   *
   * Prevents moving of `AtomicPtr` instances to maintain unique ownership semantics.
   *
   * 防止移动 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr(AtomicPtr&&) = delete;

  /**
   * @brief Deleted move assignment operator.
   * @brief 删除的移动赋值运算符。
   *
   * Prevents move assignment of `AtomicPtr` instances to maintain unique ownership semantics.
   *
   * 防止移动赋值 `AtomicPtr` 实例以保持唯一所有权语义。
   */
  AtomicPtr& operator=(AtomicPtr&&) = delete;

  /**
   * @brief Destructor.
   * @brief 析构函数。
   *
   * Default destructor ensures proper cleanup of the shared pointer.
   *
   * 默认析构函数确保正确清理共享指针。
   */
  ~AtomicPtr() = default;

  /**
   * @brief Assigns new data to the atomic pointer.
   * @brief 赋予原子指针新的数据。
   *
   * Creates a new `std::shared_ptr<const T>` pointing to a copy of `data` and atomically stores it.
   *
   * 创建一个指向 `data` 复制品的新的 `std::shared_ptr<const T>` 并原子地存储它。
   *
   * @param data The new data to be stored.
   * @param data 要存储的新数据。
   */
  void operator=(const T& data) { data_ptr_.store(std::make_shared<const T>(data)); }

  /**
   * @brief Retrieves the shared pointer.
   * @brief 获取共享指针。
   *
   * Atomically loads and returns the current `std::shared_ptr<const T>`.
   *
   * 原子加载并返回当前的 `std::shared_ptr<const T>`。
   *
   * @return A shared pointer to the constant object of type `T`.
   * @return 一个指向类型 `T` 常量对象的共享指针。
   */
  std::shared_ptr<const T> GetPtr() const { return data_ptr_.load(); }

  /**
   * @brief Retrieves the value pointed to by the shared pointer.
   * @brief 获取共享指针指向的值。
   *
   * Atomically loads the shared pointer and returns a copy of the object it points to.
   *
   * 原子加载共享指针并返回它指向的对象的副本。
   *
   * @return A copy of the object of type `T`.
   * @return 类型 `T` 对象的副本。
   */
  T GetValue() const { return *data_ptr_.load(); }

 private:
  std::atomic<std::shared_ptr<const T>> data_ptr_; /**< Atomic shared pointer to a constant object of type `T` */
};

}  // namespace openrobot::ocm
// end of selection