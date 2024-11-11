# 查找所有源文件
file(GLOB OCM_SRC_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

add_executable(ocm-gen ${OCM_SRC_FILES})
set_target_properties(ocm-gen PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ocm-gen PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # 安装时的头文件路径
)
target_link_libraries(ocm-gen PRIVATE GLib2::glib)

install(TARGETS ocm-gen
  EXPORT ocmTargets
  RUNTIME DESTINATION bin
)

