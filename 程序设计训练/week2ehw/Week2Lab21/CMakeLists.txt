cmake_minimum_required(VERSION 3.10)
project(Week2Lab21)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Main Main.cpp Worker.hpp)

target_link_libraries(Main Qt5::Widgets)
