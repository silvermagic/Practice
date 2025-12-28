cmake_minimum_required(VERSION 3.10)
project(unix_system LANGUAGES C CXX)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置 C++ 标准（供测试程序使用）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集所有源文件
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

# 创建共享库
add_library(unix_system SHARED ${SRC_FILES})

# 设置包含目录
target_include_directories(unix_system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 设置库的输出目录
set_target_properties(unix_system PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# 添加测试子目录
add_subdirectory(test)