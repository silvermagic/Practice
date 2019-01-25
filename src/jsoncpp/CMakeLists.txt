### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

### 可执行文件名、源文件列表
add_executable(app main.cc)

### 设置链接库
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})
link_directories(${JSONCPP_LIBRARY_DIRS})
target_link_libraries(app ${JSONCPP_LIBRARIES})