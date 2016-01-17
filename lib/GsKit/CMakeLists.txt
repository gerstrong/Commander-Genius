# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 2.8.12)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

IF(WIN32)
	ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

IF (OPENGL)
        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)


# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
        ADD_DEFINITIONS(-DDEBUG)
        ADD_DEFINITIONS(-g)
        ADD_DEFINITIONS(-O0)
        ADD_DEFINITIONS(-Wall)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
        ADD_DEFINITIONS(-DRELEASE)
        ADD_DEFINITIONS(-O3)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")


Project(GsKit)

# From this point files are globbed for compilation and project setup
file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})



