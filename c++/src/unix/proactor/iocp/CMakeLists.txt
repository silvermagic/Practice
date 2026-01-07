cmake_minimum_required(VERSION 3.10)
project(echo_srv)

# 仅支持 Windows 平台
if(NOT WIN32)
    message(FATAL_ERROR "IOCP 仅支持 Windows 平台")
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp)

# 链接 Windows Socket 库
target_link_libraries(${PROJECT_NAME} Ws2_32)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

