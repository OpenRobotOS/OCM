# 提取获取父目录命名空间的公共代码
macro(get_parent_namespace parent_namespace)
  get_directory_property(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_DIRECTORY)
  get_property(${parent_namespace} DIRECTORY ${PARENT_DIR} PROPERTY OPENROBOT_NAMESPACE)
  if(NOT ${parent_namespace})
    message(FATAL_ERROR "Could not find a valid OPENROBOT_NAMESPACE in parent directory: ${PARENT_DIR}")
  endif()
endmacro()

# 设置当前目录的命名空间（根目录设置命名空间）
macro(set_main_namespace namespace)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY OPENROBOT_NAMESPACE ${namespace})
endmacro()

# 自动设置当前目录的命名空间，基于父目录命名空间和当前目录名
macro(set_namespace)
  # 获取父目录命名空间
  get_parent_namespace(PARENT_NAMESPACE)

  # 获取当前目录名并生成命名空间
  string(REGEX REPLACE ".*/\(.*\)" "\\1" CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR})

  # 合并父目录命名空间和当前目录名
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY OPENROBOT_NAMESPACE ${PARENT_NAMESPACE}::${CURRENT_DIR_NAME})
endmacro()

# 获取当前目录的命名空间
macro(get_namespace result_var)
  get_property(${result_var} DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY OPENROBOT_NAMESPACE)
  if(NOT ${result_var})
    message(FATAL_ERROR "No OPENROBOT_NAMESPACE set for directory: ${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endmacro()
