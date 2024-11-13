# 最低要求的 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(BusyWaitExample)

# 设置 C++ 标准为 C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找并启用线程库
find_package(Threads REQUIRED)

# 创建可执行文件，假设你的源文件名为 main.cpp
add_executable(busy_waiting main.cpp)

# 为可执行文件添加编译选项以启用 SSE 指令集
# 这对于使用 _mm_pause() 是必要的
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(busy_waiting PRIVATE -msse2)
elseif (MSVC)
    # 对于 MSVC，_mm_pause() 默认可用，无需额外标志
    # 你可以根据需要添加特定的编译选项
endif()

# 链接线程库
target_link_libraries(busy_waiting PRIVATE Threads::Threads)
