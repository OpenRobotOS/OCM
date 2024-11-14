cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()
project(OCM VERSION 1.0 DESCRIPTION "One Communication Middleware" LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/opt/openrobotlib" CACHE PATH "Default install path" FORCE)
endif()

option(SPDLOG_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(SPDLOG_INSTALL "Generate the install target" ${SPDLOG_MASTER_PROJECT})
option(SPDLOG_USE_STD_FORMAT "Use std::format instead of fmt library." ON)
option(SPDLOG_FMT_EXTERNAL "Use external fmt library instead of bundled" OFF)
option(SPDLOG_FMT_EXTERNAL_HO "Use external fmt header-only library instead of bundled" OFF)
option(SPDLOG_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any spdlog exceptions" OFF)
option(SPDLOG_BUILD_ALL "Build all artifacts" OFF)
# build shared option
option(SPDLOG_BUILD_SHARED "Build shared library" OFF)
# precompiled headers option
option(SPDLOG_ENABLE_PCH "Build static or shared library using precompiled header to speed up compilation time" OFF)

# build position independent code
option(SPDLOG_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# example options
option(SPDLOG_BUILD_EXAMPLE "Build example" ${SPDLOG_MASTER_PROJECT})
option(SPDLOG_BUILD_EXAMPLE_HO "Build header only example" OFF)

# testing options
option(SPDLOG_BUILD_TESTS "Build tests" OFF)
option(SPDLOG_BUILD_TESTS_HO "Build tests using the header only version" OFF)

# bench options
option(SPDLOG_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# sanitizer options
option(SPDLOG_SANITIZE_ADDRESS "Enable address sanitizer in tests" OFF)
option(SPDLOG_SANITIZE_THREAD "Enable thread sanitizer in tests" OFF)
if(SPDLOG_SANITIZE_ADDRESS AND SPDLOG_SANITIZE_THREAD)
    message(FATAL_ERROR "SPDLOG_SANITIZE_ADDRESS and SPDLOG_SANITIZE_THREAD are mutually exclusive")
endif()

# warning options
option(SPDLOG_BUILD_WARNINGS "Enable compiler warnings" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(lcm REQUIRED HINTS "/opt/openrobotlib/lib/lcm/cmake")

add_compile_options(-no-pie -O3 -march=native)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_library(OCM src/ocm.cpp)

# 1. 指定头文件路径
target_include_directories(OCM PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # 安装时的头文件路径
)

# 2. 链接外部库
target_link_libraries(OCM ${LCM_NAMESPACE}lcm)

# 3. 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# 4. 安装库文件
install(TARGETS OCM
    EXPORT OCMTargets
    RUNTIME DESTINATION bin    # 如果是可执行文件（.exe / .out）
    LIBRARY DESTINATION lib    # 如果是动态库（.so / .dll）
    ARCHIVE DESTINATION lib    # 如果是静态库（.a）
    INCLUDES DESTINATION include
)

# 5. 导出目标信息
install(EXPORT OCMTargets
    FILE OCMTargets.cmake
    NAMESPACE OPENROBOT::
    DESTINATION lib/cmake/OCM
)

# 6. 配置并安装包配置文件
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_MODULE_PATH}/OCMConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/OCM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OCMConfigVersion.cmake"
    DESTINATION lib/cmake/OCM
)
