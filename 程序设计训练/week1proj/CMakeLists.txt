cmake_minimum_required(VERSION 3.10)
project(MCS)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

find_package(Eigen3)

INCLUDE_DIRECTORIES(${EIGEN3_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories(.)

add_executable(Main Main.cpp
        MainWindow.cpp MainWindow.hpp
        DesignDialog.cpp DesignDialog.hpp ConfigureDialog.cpp ConfigureDialog.hpp Solver.cpp Solver.hpp MCSFrame.cpp MCSFrame.hpp DoubleToColor.hpp DoubleToColor.cpp)

target_link_libraries(Main Qt5::Widgets)