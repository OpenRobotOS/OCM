# OCM - One Communication Middleware

## 1. 项目介绍 / Project Introduction
OCM 是一个用于机器人通信的中间件，提供了一个通用的通信、任务执行框架，支持多种通信协议和数据格式。  
OCM is a middleware for robot communication, providing a general communication and task execution framework that supports multiple communication protocols and data formats.

## 2. 项目结构 / Project Structure

### 2.1 通信 / Communication

#### 2.1.1 进程内通信 / Intra-process Communication
- `ocm/atomic_ptr.hpp`：原子指针，提供线程安全的指针操作。  Atomic pointer, provides thread-safe pointer operations.
- `ocm/writer_reader_lock.hpp`：读写锁，提供读写锁操作。  Read-write lock, provides read-write lock operations.
- 参照`examples/intra-process`：进程内通信示例。  Refer to `examples/intra-process`: Intra-process communication example.

#### 2.1.2 进程间通信 / Inter-process Communication
- `ocm/shared_memory_topic.hpp`：共享内存主题，提供共享内存发布订阅功能。  Shared memory topic, provides shared memory publish-subscribe functionality.
- 参照`examples/inter-process`：进程间通信示例。  Refer to `examples/inter-process`: Inter-process communication example.

#### 2.1.3 设备间通信 / Inter-device Communication
- [LCM](https://lcm-proj.github.io/lcm/)  
- 参照`examples/inter-device`：设备间通信示例。  Refer to `examples/inter-device`: Inter-device communication example.

#### 2.1.4 序列化 / Serialization
- [LCM](https://lcm-proj.github.io/lcm/)  

### 2.2 任务执行 / Task Execution

#### 2.2.1 节点管理 / Node Management
- `node/node.hpp`：节点，最小执行单元。  Node, the smallest execution unit.
- `node/node_map.hpp`：节点管理，提供节点管理功能。  Node management, provides node management functionality.
- 参照`examples/node`：节点示例。  Refer to `examples/node`: Node example.

#### 2.2.2 任务管理 / Task Management
- `task/task_base.hpp`：任务基类，提供定时器、线程管理功能。  Task base class, provides timer and thread management functionality.
- 参照`examples/task`：任务示例。  Refer to `examples/task`: Task example.

#### 2.2.3 调度器 / Scheduler
- `executer/executer.hpp`：调度器，提供任务调度功能。  Scheduler, provides task scheduling functionality.
- 参照`examples/executer`：调度器示例。  Refer to `examples/executer`: Scheduler example.

## 2.3 日志 / Logging
- `log_anywhere/log_anywhere.hpp`：提供进程内共享的异步日志功能。  Provides shared asynchronous logging within the process.
- [spdlog](https://github.com/gabime/spdlog)  
- 参照`examples/log_anywhere`：日志示例。  Refer to `examples/log_anywhere`: Logging example.

## 2.4 数据调试 / Data Debugging
- `debug_anywhere/debug_anywhere.hpp`：提供进程内共享的调试数据异步发布功能。  Provides shared asynchronous publishing of debug data within the process.
- 参照`examples/debug_anywhere`：数据调试示例。  Refer to `examples/debug_anywhere`: Data debugging example.

## 2.5 参数 / Parameters
- 参照`parameter_generator/ocm-parmgen.py`：提供参数的C++数据类自动生成工具。  Refer to `parameter_generator/parameter`: Provides C++ data class auto-generation tools for parameters.

## 3. 依赖 / Dependencies
- [LCM](https://lcm-proj.github.io/lcm/)
- [spdlog](https://github.com/gabime/spdlog)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [boost](https://www.boost.org/)

## 4. 环境要求 / Environment Requirements
- gcc >= 13
- cmake >= 3.10

## 5. 编译 / Compilation
```bash
mkdir build
cd build
cmake ..
sudo make install -j # 默认安装到/opt/openrobotlib/ocm，默认依赖位置/opt/openrobotlib/third_party
```
```
