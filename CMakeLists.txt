cmake_minimum_required(VERSION 3.10)
project(ocm VERSION 1.0 DESCRIPTION "One Communication Middleware" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-no-pie -O3 -march=native)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# 添加子目录
add_subdirectory(ocm)

