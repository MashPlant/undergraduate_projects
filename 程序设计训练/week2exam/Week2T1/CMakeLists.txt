cmake_minimum_required(VERSION 3.10)
project(Week2T1)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Main Main.cpp
        Dialog.cpp Dialog.hpp Server.cpp Server.hpp)

target_link_libraries(Main Qt5::Widgets Qt5::Network)