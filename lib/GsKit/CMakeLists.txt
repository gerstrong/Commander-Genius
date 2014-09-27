# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

# Make this C++11. We need that in GsKit
ADD_DEFINITIONS(-std=c++11)

IF(WIN32)
	ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

Project(GsKit)

# TODO: From this point we should recursively glob the files. I think it is the easiest way but maybe not the best, I don't know yet.
file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})



