# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.6)

MESSAGE( "Preparing the Build-System for Commander Genius" )
EXEC_PROGRAM(sh ARGS ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE CG_VERSION)
string(REGEX REPLACE "[\r\n]" "" CG_VERSION "${CG_VERSION}")

INCLUDE(build.cmake)

INCLUDE(install.cmake)

INCLUDE(package.cmake)
