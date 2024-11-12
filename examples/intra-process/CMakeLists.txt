cmake_minimum_required(VERSION 3.14)

project(UsingOCMExample LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找 OCM 包
find_package(OCM REQUIRED)

# 添加可执行文件
add_executable(UsingOCM main.cpp)

# 链接 OCM 库
target_link_libraries(UsingOCM PRIVATE OCM::OCM)


