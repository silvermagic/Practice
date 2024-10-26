### 设置最低 CMake 版本
cmake_minimum_required(VERSION 3.15)

### 设置工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_STANDARD 17)  # 使用 C++17 标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 确保 C++11 标准是必需的
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用编译器特定的扩展
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 启用生成 compile_commands.json 文件

# 设置编译选项
add_compile_options(-Wall)  # 启用所有警告

# 设置调试模式下的编译选项
set(CMAKE_CXX_FLAGS_DEBUG "-g")

### 设置头文件目录
include_directories(/usr/local/include)

### 查找 Boost 库
find_package(Boost REQUIRED COMPONENTS system)

### 检查 Boost 是否找到
if(Boost_FOUND)
    add_executable(app main.cc chat.cc)
    target_include_directories(app PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(app 
        PRIVATE 
        ${Boost_LIBRARIES} 
        -Wl,--whole-archive muduo_base muduo_net -Wl,--no-whole-archive 
        pthread)
    add_executable(cli client.cc)
    target_include_directories(cli PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(cli 
        PRIVATE 
        ${Boost_LIBRARIES} 
        -Wl,--whole-archive muduo_base muduo_net -Wl,--no-whole-archive 
        pthread)
else()
    message(FATAL_ERROR "Boost library not found!")
endif()