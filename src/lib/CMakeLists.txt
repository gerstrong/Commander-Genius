# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

project(GsKit)

# TODO: From this point we should recursively glob the files. I think it is the easiest way...

file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})
