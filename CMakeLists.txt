# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 3.5)

Project(CommanderGenius)

option(USE_SDL_TTF "Use true type fonts" yes)

IF (USE_SDL_TTF)
    ADD_DEFINITIONS(-DUSE_SDL_TTF)
ENDIF(USE_SDL_TTF)

option(USE_VIRTUALPAD "Enable Onscreen Virtual Gamepad support " ON)

if(USE_VIRTUALPAD)
    ADD_DEFINITIONS(-DVIRTUALPAD)
endif(USE_VIRTUALPAD)


OPTION(DUMP_MTCS "Show Mouse Touch Cursor Position" No)

IF (DUMP_MTCS)
    ADD_DEFINITIONS(-DDUMP_MTCS)
endif(DUMP_MTCS)


# Add GsKit
add_subdirectory("GsKit")

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

# Modules path located at GsKit/CMake
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/GsKit/CMake")

# Use COmpile TIme REducer
include(GsKit/CMake/cotire.cmake)

option(USE_PYTHON3 "Use Python3 scripts" no)

set (CMAKE_CXX_STANDARD 14)

#------------------------------------------------------------------------------
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

MESSAGE(STATUS "Preparing the Build-System for Commander Genius" )
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE CG_VERSION)
string(REGEX REPLACE "[\r\n]" "" CG_VERSION "${CG_VERSION}")

# Since shell script get_version is not processed properly on windows, we have to define manually here.
# TODO: Better system for defining the version
IF(WIN32)
  set(CG_VERSION "2.3.6")
endif()

# Generate the README file with the correct version string
configure_file(README.in README)
configure_file(README.in ${CMAKE_BINARY_DIR}/README)
configure_file(README.in ${CMAKE_CURRENT_SOURCE_DIR}/README)
configure_file(README.in ${CMAKE_CURRENT_SOURCE_DIR}/debian/README)

set(CMAKE_C_FLAGS "-std=c99")

# Pass correct definition for SYSTEM_DATA_DIR so packager or distributors can decide where the app should land in
if(UNIX)
        if(NOT IS_ABSOLUTE ${GAMES_SHAREDIR})
                set(FULL_GAMES_SHAREDIR "${CMAKE_INSTALL_PREFIX}/${GAMES_SHAREDIR}")
        else()
                set(FULL_GAMES_SHAREDIR ${GAMES_SHAREDIR})
        endif()

        MESSAGE(STATUS "Setting SYSTEM_DATA_DIR to ${FULL_GAMES_SHAREDIR}" )

        add_definitions(-DSYSTEM_DATA_DIR="${FULL_GAMES_SHAREDIR}")
endif(UNIX)


# Check that GsKit also has been loaded
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/GsKit/CMakeLists.txt")

    MESSAGE(FATAL_ERROR "You need to have the sources of GsKit in order to build Commander Genius. ")

endif()


# And build Commander Genius on top of that!
add_subdirectory("src")

MESSAGE(STATUS "CG_VERSION = ${CG_VERSION}" )

INCLUDE(package.cmake)

MESSAGE( STATUS "Build system is prepared. To Build the project just type \"make\"" )
MESSAGE( STATUS "If you want to create the installation package just type \"make package\" after you build the project" )

