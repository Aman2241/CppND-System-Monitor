cmake_minimum_required(VERSION 3.15.0)

# set(CMAKE_CXX_STANDARD 17)
project(System_Monitor)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(system_monitor main.cpp)
target_link_libraries(system_monitor ${CURSES_LIBRARIES})