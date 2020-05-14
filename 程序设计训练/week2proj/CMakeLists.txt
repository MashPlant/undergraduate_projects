cmake_minimum_required(VERSION 3.10)
project(Chess)

#set(CMAKE_PREFIX_PATH "/home/mashplant/Qt5.9.6/5.9.6/gcc_64")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network Multimedia REQUIRED)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(Server Server.cpp MainWindow.cpp MainWindow.hpp ChessFrame.cpp ChessFrame.hpp resource.qrc IPDialog.cpp IPDialog.hpp)
add_executable(Client Client.cpp MainWindow.cpp MainWindow.hpp ChessFrame.cpp ChessFrame.hpp resource.qrc IPDialog.cpp IPDialog.hpp)

target_link_libraries(Server Qt5::Widgets Qt5::Network Qt5::Multimedia)
target_link_libraries(Client Qt5::Widgets Qt5::Network Qt5::Multimedia)
