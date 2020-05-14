cmake_minimum_required(VERSION 3.10)
project(Week1T3)

set(Qt5MultiMedia_DIR "/home/mashplant/Qt5.9.6/5.9.6/gcc_64/lib/cmake/Qt5Multimedia")
set(CMAKE_PREFIX_PATH "/home/mashplant/Qt5.9.6/5.9.6/gcc_64/lib/cmake/Qt5Multimedia")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5MultiMedia REQUIRED)


set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Main Main.cpp
        MainWindow.cpp MainWindow.hpp)

target_link_libraries(Main Qt5::Widgets Qt5MultiMedia)