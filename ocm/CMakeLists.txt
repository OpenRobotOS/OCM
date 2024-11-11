add_library(OCM STATIC src/ocm.cpp)
target_include_directories(OCM PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # 安装时的头文件路径
)

# 安装 ocm 库
install(TARGETS OCM
    EXPORT ocmTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# 导出 CMake 目标文件
install(EXPORT ocmTargets
    FILE ocmTargets.cmake
    DESTINATION lib/cmake/ocm
)

# 配置并安装 CMake 配置文件
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_MODULE_PATH}/OCMConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake
    INSTALL_DESTINATION lib/cmake/ocm
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/OCMConfig.cmake
    DESTINATION lib/cmake/ocm
)
