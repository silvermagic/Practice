cmake_minimum_required(VERSION 3.16.3)

project(test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fpermissive -O3")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "BOOST ${Boost_LIB_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

file(GLOB_RECURSE SRC "*.cc")

add_executable(test ${SRC})

target_link_libraries(test ${Boost_LIBRARIES})