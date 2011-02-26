# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.6)

MESSAGE( "Preparing the Build-System for Commander Genius" )
SET(CG_VERSION "0.3.3.1 Pre")

INCLUDE(build.cmake)

INCLUDE(install.cmake)

INCLUDE(package.cmake)
