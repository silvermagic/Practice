cmake_minimum_required(VERSION 2.8)
project(pwd)

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(pwd ${SRC})

#dependencies
#INCLUDE(FindPkgConfig)
#pkg_check_modules(XX REQUIRED XXX)
#include_directories(${XX_INCLUDE_DIRS})
#target_link_libraries(Practice ${XX_LIBRARIES})

