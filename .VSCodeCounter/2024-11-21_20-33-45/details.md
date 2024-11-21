# Details

Date : 2024-11-21 20:33:45

Directory /home/lizhen/works/code/OpenRobot/OCM

Total : 53 files,  4260 codes, 547 comments, 848 blanks, all 5655 lines

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [.clang-format](/.clang-format) | YAML | 12 | 0 | 3 | 15 |
| [README.md](/README.md) | Markdown | 2 | 0 | 1 | 3 |
| [examples/executer/CMakeLists.txt](/examples/executer/CMakeLists.txt) | cmake | 15 | 0 | 6 | 21 |
| [examples/executer/executer.cpp](/examples/executer/executer.cpp) | C++ | 100 | 1 | 9 | 110 |
| [examples/executer/node_test.hpp](/examples/executer/node_test.hpp) | C++ | 84 | 0 | 6 | 90 |
| [examples/executer/yaml_load_generated_classes.hpp](/examples/executer/yaml_load_generated_classes.hpp) | C++ | 370 | 14 | 87 | 471 |
| [examples/executer/yaml_template/task/task_setting.yaml](/examples/executer/yaml_template/task/task_setting.yaml) | YAML | 70 | 6 | 2 | 78 |
| [examples/inter-device/CMakeLists.txt](/examples/inter-device/CMakeLists.txt) | cmake | 24 | 0 | 9 | 33 |
| [examples/inter-device/listener.cpp](/examples/inter-device/listener.cpp) | C++ | 35 | 10 | 10 | 55 |
| [examples/inter-device/read_log.cpp](/examples/inter-device/read_log.cpp) | C++ | 39 | 18 | 12 | 69 |
| [examples/inter-device/send_message.cpp](/examples/inter-device/send_message.cpp) | C++ | 25 | 9 | 11 | 45 |
| [examples/inter-process/CMakeLists.txt](/examples/inter-process/CMakeLists.txt) | cmake | 18 | 0 | 7 | 25 |
| [examples/inter-process/MyData.hpp](/examples/inter-process/MyData.hpp) | C++ | 84 | 35 | 35 | 154 |
| [examples/inter-process/sem.cpp](/examples/inter-process/sem.cpp) | C++ | 40 | 10 | 14 | 64 |
| [examples/inter-process/shm.cpp](/examples/inter-process/shm.cpp) | C++ | 31 | 10 | 14 | 55 |
| [examples/intra-process/AtomicPtr.cpp](/examples/intra-process/AtomicPtr.cpp) | C++ | 16 | 3 | 6 | 25 |
| [examples/intra-process/CMakeLists.txt](/examples/intra-process/CMakeLists.txt) | cmake | 16 | 0 | 9 | 25 |
| [examples/intra-process/RWLockData.cpp](/examples/intra-process/RWLockData.cpp) | C++ | 24 | 3 | 6 | 33 |
| [examples/logger_test/CMakeLists.txt](/examples/logger_test/CMakeLists.txt) | cmake | 16 | 0 | 6 | 22 |
| [examples/logger_test/logger.cpp](/examples/logger_test/logger.cpp) | C++ | 14 | 4 | 6 | 24 |
| [examples/node/CMakeLists.txt](/examples/node/CMakeLists.txt) | cmake | 16 | 0 | 6 | 22 |
| [examples/node/test.cpp](/examples/node/test.cpp) | C++ | 33 | 0 | 4 | 37 |
| [examples/parameter/CMakeLists.txt](/examples/parameter/CMakeLists.txt) | cmake | 16 | 0 | 6 | 22 |
| [examples/parameter/generated_classes.hpp](/examples/parameter/generated_classes.hpp) | C++ | 370 | 11 | 87 | 468 |
| [examples/parameter/paratest.cpp](/examples/parameter/paratest.cpp) | C++ | 11 | 3 | 5 | 19 |
| [examples/parameter/test copy 2.py](/examples/parameter/test%20copy%202.py) | Python | 136 | 24 | 23 | 183 |
| [examples/parameter/test copy 3.py](/examples/parameter/test%20copy%203.py) | Python | 160 | 31 | 32 | 223 |
| [examples/parameter/test copy 4.py](/examples/parameter/test%20copy%204.py) | Python | 136 | 24 | 23 | 183 |
| [examples/parameter/test copy 5.py](/examples/parameter/test%20copy%205.py) | Python | 178 | 35 | 28 | 241 |
| [examples/parameter/test copy 6.py](/examples/parameter/test%20copy%206.py) | Python | 338 | 77 | 53 | 468 |
| [examples/parameter/test copy.py](/examples/parameter/test%20copy.py) | Python | 97 | 13 | 23 | 133 |
| [examples/parameter/test.py](/examples/parameter/test.py) | Python | 345 | 77 | 53 | 475 |
| [examples/task/CMakeLists.txt](/examples/task/CMakeLists.txt) | cmake | 20 | 0 | 5 | 25 |
| [examples/task/external_timer.cpp](/examples/task/external_timer.cpp) | C++ | 34 | 0 | 7 | 41 |
| [examples/task/internal_timer.cpp](/examples/task/internal_timer.cpp) | C++ | 15 | 0 | 4 | 19 |
| [examples/task/trigger.cpp](/examples/task/trigger.cpp) | C++ | 19 | 0 | 3 | 22 |
| [examples/test.cpp](/examples/test.cpp) | C++ | 20 | 7 | 9 | 36 |
| [examples/test2.cpp](/examples/test2.cpp) | C++ | 34 | 10 | 12 | 56 |
| [ocm/CMakeLists.txt](/ocm/CMakeLists.txt) | cmake | 87 | 0 | 19 | 106 |
| [ocm/include/common/enum.hpp](/ocm/include/common/enum.hpp) | C++ | 18 | 0 | 3 | 21 |
| [ocm/include/common/struct_type.hpp](/ocm/include/common/struct_type.hpp) | C++ | 41 | 0 | 2 | 43 |
| [ocm/include/executer/executer.hpp](/ocm/include/executer/executer.hpp) | C++ | 234 | 0 | 13 | 247 |
| [ocm/include/log_anywhere/log_anywhere.hpp](/ocm/include/log_anywhere/log_anywhere.hpp) | C++ | 72 | 16 | 19 | 107 |
| [ocm/include/node/node.hpp](/ocm/include/node/node.hpp) | C++ | 30 | 0 | 5 | 35 |
| [ocm/include/node/node_map.hpp](/ocm/include/node/node_map.hpp) | C++ | 27 | 0 | 5 | 32 |
| [ocm/include/ocm/ocm.hpp](/ocm/include/ocm/ocm.hpp) | C++ | 120 | 9 | 26 | 155 |
| [ocm/include/parameter/parameter.hpp](/ocm/include/parameter/parameter.hpp) | C++ | 27 | 7 | 7 | 41 |
| [ocm/include/task/rt/memory_lock.hpp](/ocm/include/task/rt/memory_lock.hpp) | C++ | 93 | 3 | 15 | 111 |
| [ocm/include/task/rt/sched_rt.hpp](/ocm/include/task/rt/sched_rt.hpp) | C++ | 81 | 4 | 17 | 102 |
| [ocm/include/task/task.hpp](/ocm/include/task/task.hpp) | C++ | 87 | 0 | 18 | 105 |
| [ocm/include/task/timer.hpp](/ocm/include/task/timer.hpp) | C++ | 63 | 21 | 14 | 98 |
| [ocm/src/ocm.cpp](/ocm/src/ocm.cpp) | C++ | 146 | 52 | 27 | 225 |
| [ocm/src/task.cpp](/ocm/src/task.cpp) | C++ | 121 | 0 | 16 | 137 |

[Summary](results.md) / Details / [Diff Summary](diff.md) / [Diff Details](diff-details.md)