### cmake的最低版本
cmake_minimum_required(VERSION 2.8)
### 工程名
project(app)

### 设置编译参数
set(CMAKE_CXX_FLAGS  "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG  "-g")

### 执行自定义脚本
execute_process(COMMAND glib-genmarshal --header --prefix=g_cclosure_user_marshal ${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom-marshal.h)
execute_process(COMMAND glib-genmarshal --body --prefix=g_cclosure_user_marshal ${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/custom-marshal.c)

### 可执行文件名、源文件列表
file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(app ${SRC})

### 设置链接库
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
include_directories(${GLIB_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS} ${GOBJECT_LIBRARY_DIRS})
target_link_libraries(app ${GLIB_LIBRARIES} ${GOBJECT_LIBRARIES})
