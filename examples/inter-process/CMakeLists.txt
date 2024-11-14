cmake_minimum_required(VERSION 3.14)

project(UsingOCMExample LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "/opt/openrobotlib" CACHE PATH "Default prefix path" FORCE)
endif()

# 查找 OCM 包
find_package(OCM REQUIRED)
find_package(lcm REQUIRED)
include(${LCM_USE_FILE})

# 添加可执行文件
add_executable(shm shm.cpp)
# add_executable(RWLockData RWLockData.cpp)


# 链接 OCM 库
target_link_libraries(shm PRIVATE OPENROBOT::OCM ${LCM_NAMESPACE}lcm)
# target_link_libraries(RWLockData PRIVATE OPENROBOT::OCM)
