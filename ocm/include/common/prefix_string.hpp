// Start of Selection
#pragma once
#include <string>

namespace openrobot::ocm {
/**
 * @brief 命名前缀
 * @brief Name prefix
 */
#define NAME_PREFIX "openrobot_ocm_"

/**
 * @brief 获取带前缀的名称
 * @brief Gets the name with prefix.
 *
 * @param name 原始名称。
 * @param name The original name.
 * @return 返回带前缀的名称。
 * @return A string containing the name with prefix.
 */
inline std::string GetNamePrefix(const std::string& name) { return NAME_PREFIX + name; }

}  // namespace openrobot::ocm
// End of Selection