### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

### 可执行文件名、源文件列表
add_executable(app main.cc)