### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_FLAGS  "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

### 可执行文件名、源文件列表
add_executable(producer producer.cc)
add_executable(consumer consumer.cc)

### 设置链接库
find_package(PkgConfig REQUIRED)
pkg_check_modules(RABBITMQ REQUIRED librabbitmq)
include_directories(${RABBITMQ_INCLUDE_DIRS})
link_directories(${RABBITMQ_LIBRARY_DIRS})
target_link_libraries(producer ${RABBITMQ_LIBRARIES})
target_link_libraries(consumer ${RABBITMQ_LIBRARIES})