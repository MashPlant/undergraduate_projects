cmake_minimum_required(VERSION 3.10)
project(Week1T2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Main Main.cpp
        MainWindow.cpp MainWindow.hpp)

target_link_libraries(Main Qt5::Widgets)