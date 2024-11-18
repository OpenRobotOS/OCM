cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()
project(
  OCM
  VERSION 1.0
  DESCRIPTION "One Communication Middleware"
  LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Choose Release or Debug" FORCE)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "/opt/openrobotlib"
      CACHE PATH "Default install path" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(lcm REQUIRED HINTS "/opt/openrobotlib/lib/lcm/cmake")
find_package(spdlog REQUIRED HINTS "/opt/openrobotlib/lib/cmake/spdlog")
find_package(yaml-cpp REQUIRED HINTS "/opt/openrobotlib/lib/cmake/yaml-cpp")

add_compile_options(-no-pie -O3 -march=native)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_library(OCM STATIC src/ocm.cpp src/task.cpp)

# 1. 指定头文件路径
target_include_directories(
  OCM PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
             $<INSTALL_INTERFACE:include> # 安装时的头文件路径
)

# 1. 链接外部库
target_link_libraries(OCM PUBLIC ${LCM_NAMESPACE}lcm spdlog::spdlog
                                 yaml-cpp::yaml-cpp)

# 1. 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# 1. 安装库文件
install(
  TARGETS OCM
  EXPORT OCMTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES
  DESTINATION include)

# 1. 导出目标信息
install(
  EXPORT OCMTargets
  FILE OCMTargets.cmake
  NAMESPACE OPENROBOT::
  DESTINATION lib/cmake/OCM)

# 1. 配置并安装包配置文件
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${CMAKE_MODULE_PATH}/OCMConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
  INSTALL_DESTINATION lib/cmake/OCM)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
        DESTINATION lib/cmake/OCM)
