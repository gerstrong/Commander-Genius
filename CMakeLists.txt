# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 3.5)

Project(CommanderGenius)



if (OSXCROSS)
message(STATUS "OSXCross is enabled")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


option(USE_VIRTUALPAD "Enable Onscreen Virtual Gamepad support " ON)

if(USE_VIRTUALPAD)
    ADD_DEFINITIONS(-DUSE_VIRTUALPAD)
endif(USE_VIRTUALPAD)

# In case you want a switch build
OPTION(NINTENDO_SWITCH "Build for Nintendo Switch" No)

if(NINTENDO_SWITCH)
    ADD_DEFINITIONS(-D__SWITCH__)
endif()


# GP2X builds
OPTION(GP2X "Build for GP2X" No)

if(GP2X)
    ADD_DEFINITIONS(-D__GP2X__)
endif()


OPTION(DUMP_MTCS "Show Mouse Touch Cursor Position" No)

IF (DUMP_MTCS)
    ADD_DEFINITIONS(-DDUMP_MTCS)
endif(DUMP_MTCS)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache) # Less useful to do it for linking, see edit2
    message(STATUS "Using ccache for faster builds")
endif(CCACHE_FOUND)

# Will pass the SHA and branch strings to the executable
find_program(GIT_FOUND git)
if(GIT_FOUND)
    message(STATUS "Found Git (${GIT_FOUND}), executable will get SHA and branch used")
    execute_process(COMMAND ${GIT_FOUND} -C ${CMAKE_SOURCE_DIR} rev-parse --short HEAD
                    OUTPUT_VARIABLE GIT_SHA
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GIT_FOUND} -C ${CMAKE_SOURCE_DIR} rev-parse --abbrev-ref HEAD
                    OUTPUT_VARIABLE GIT_BRANCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    ADD_DEFINITIONS(-DGIT_SHA=${GIT_SHA})
    ADD_DEFINITIONS(-DGIT_BRANCH=${GIT_BRANCH})
    message(STATUS "GIT_SHA = ${GIT_SHA}")
    message(STATUS "GIT_BRANCH = ${GIT_BRANCH}")
else(GIT_FOUND)
    ADD_DEFINITIONS(-DGIT_SHA=none)
    ADD_DEFINITIONS(-DGIT_BRANCH=none)
endif(GIT_FOUND)


# Add GsKit
add_subdirectory("GsKit")

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

# Modules path located at GsKit/CMake
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/GsKit/CMake")

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
  set(CG_VERSION "2.4.6")
endif()

# Generate the README file with the correct version string
configure_file(README.in README)
configure_file(README.in ${CMAKE_BINARY_DIR}/README)
configure_file(README.in ${CMAKE_CURRENT_SOURCE_DIR}/README)
configure_file(README.in ${CMAKE_CURRENT_SOURCE_DIR}/debian/README)

if(APPLE)
    file(COPY cglogo.icns DESTINATION ${CMAKE_BINARY_DIR}/src)
    file(COPY Info.plist DESTINATION ${CMAKE_BINARY_DIR}/src)
    file(COPY launchCGonMacOs.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY buildMacOsBundle.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY makebundleable.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY get_version.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY version.h DESTINATION ${CMAKE_BINARY_DIR})
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -headerpad_max_install_names")
endif(APPLE)

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

IF(APPLE)
    # At least it works with Mac ports
    # Otherwise SDL2 won't be found
    link_directories(/opt/local/lib/)
ENDIF(APPLE)

# And build Commander Genius on top of that!
add_subdirectory("src")

add_custom_target(${PROJECT_NAME} SOURCES changelog.txt README.in)

MESSAGE(STATUS "CG_VERSION = ${CG_VERSION}" )

INCLUDE(package.cmake)

MESSAGE( STATUS "Build system is prepared. To Build the project just type \"make\"" )
MESSAGE( STATUS "If you want to create the installation package just type \"make package\" after you build the project" )

