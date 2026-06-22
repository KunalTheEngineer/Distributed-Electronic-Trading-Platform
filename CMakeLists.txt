cmake_minimum_required(VERSION 3.16)

project(TradingGUI)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(app WIN32 src/main.cpp)

target_link_libraries(app Qt6::Widgets)