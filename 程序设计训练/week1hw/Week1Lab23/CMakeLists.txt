cmake_minimum_required(VERSION 3.10)
project(Week1Lab23)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Main Main.cpp
        MainWindow.cpp MainWindow.hpp
        resource.qrc)

target_link_libraries(Main Qt5::Widgets)