### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_C_FLAGS  "-Wall")
set(CMAKE_C_FLAGS_DEBUG  "-g")

### 可执行文件名、源文件列表
add_executable(app main.c)

### 设置链接库
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
target_link_libraries(app ${GTK3_LIBRARIES})