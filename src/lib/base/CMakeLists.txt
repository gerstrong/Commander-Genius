# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

project(GsKit_base)

add_library(GsKit_base GsApp.cpp
                       GsAppState.cpp
                       GsLogging.cpp
                       GsTimer.cpp
                       GsEvent.h
                       CInput.cpp)
