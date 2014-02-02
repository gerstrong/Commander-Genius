# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

include_directories ("${PROJECT_SOURCE_DIR}/GsWidgets")
add_subdirectories("GsWidgets")

project(GsKit)

target_link_libraries(GsKit GsWidgets)