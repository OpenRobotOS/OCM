cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0111)
  cmake_policy(SET CMP0111 NEW)
endif()
project(OCM VERSION 1.0 DESCRIPTION "One Communication Middleware" LANGUAGES CXX)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/opt/openrobotlib" CACHE PATH "Default install path" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-no-pie -O3 -march=native)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_library(OCM INTERFACE)
target_include_directories(OCM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # 安装时的头文件路径
)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

install(TARGETS OCM
    EXPORT OCMTargets
    INCLUDES DESTINATION include
)

install(EXPORT OCMTargets
    FILE OCMTargets.cmake
    NAMESPACE OCM::
    DESTINATION lib/cmake/OCM
)

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
