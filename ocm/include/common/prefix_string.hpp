#pragma once
#include <string>

namespace openrobot::ocm {
#define NAME_PREFIX "openrobot_ocm_"

inline std::string GetNamePrefix(const std::string& name) { return NAME_PREFIX + name; }

}  // namespace openrobot::ocm
