# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

MESSAGE( "Preparing the Build-System for Commander Genius" )
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE CG_VERSION)
string(REGEX REPLACE "[\r\n]" "" CG_VERSION "${CG_VERSION}")

INCLUDE(build.cmake)

INCLUDE(install.cmake)

# pass correct definition for SYSTEM_DATA_DIR
if(UNIX)
	if(NOT IS_ABSOLUTE ${GAMES_SHAREDIR})
		set(FULL_GAMES_SHAREDIR "${CMAKE_INSTALL_PREFIX}/${GAMES_SHAREDIR}")
	else()
		set(FULL_GAMES_SHAREDIR ${GAMES_SHAREDIR})
	endif()
	add_definitions(-DSYSTEM_DATA_DIR="${FULL_GAMES_SHAREDIR}")
endif(UNIX)

INCLUDE(package.cmake)
