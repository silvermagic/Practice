cmake_minimum_required(VERSION 3.10)
project(echo_srv)

# 仅支持 Linux 平台
if(NOT UNIX)
    message(FATAL_ERROR "epoll 仅支持 Linux/Unix 平台")
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

