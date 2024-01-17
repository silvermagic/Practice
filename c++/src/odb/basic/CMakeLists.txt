### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

### 执行自定义脚本
message("COMMAND: odb -d pgsql --output-dir ${CMAKE_CURRENT_SOURCE_DIR} --generate-query --generate-schema ${CMAKE_CURRENT_SOURCE_DIR}/person.hxx")
execute_process(COMMAND odb -d pgsql --output-dir ${CMAKE_CURRENT_SOURCE_DIR} --generate-query --generate-schema ${CMAKE_CURRENT_SOURCE_DIR}/person.hxx)
### 可执行文件名、源文件列表
file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
add_executable(app ${SRC})
target_link_libraries(app -lodb -lodb-pgsql)