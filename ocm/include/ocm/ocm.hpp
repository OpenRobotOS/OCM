#pragma once

#include <memory>
#include <stdexcept>

namespace openrobot::ocm {

template <typename T>
class AtomicData {
 public:
  AtomicData() { data_ptr_.store(std::make_shared<T>()); }
  explicit AtomicData(const T& data) { data_ptr_.store(std::make_shared<T>(data)); }
  AtomicData(const AtomicData&) { throw std::logic_error("[AtomicData]atomic data copy construct!"); }
  AtomicData& operator=(AtomicData&&) = delete;
  AtomicData& operator=(const AtomicData&) = delete;
  AtomicData(AtomicData&&) = delete;
  ~AtomicData() = default;

  void operator=(const T& data) { data_ptr_.store(std::make_shared<T>(data)); }
  std::shared_ptr<T> operator()() const { return data_ptr_.load(); }

 private:
  std::atomic<std::shared_ptr<T>> data_ptr_;
};

}  // namespace openrobot::ocm