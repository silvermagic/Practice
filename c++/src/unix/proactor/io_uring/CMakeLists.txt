cmake_minimum_required(VERSION 3.10)
project(echo_srv)

# 仅支持 Linux 平台
if(NOT UNIX)
    message(FATAL_ERROR "io_uring 仅支持 Linux 平台")
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 liburing 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBURING REQUIRED liburing)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp)

# 链接 liburing 库
target_link_libraries(${PROJECT_NAME} ${LIBURING_LIBRARIES})

# 包含头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBURING_INCLUDE_DIRS})

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

