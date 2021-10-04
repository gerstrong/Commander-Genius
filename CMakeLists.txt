# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally.

cmake_minimum_required(VERSION 3.7)

Project(CommanderGenius)

IF(WIN32)
SET(DATADIR CGenius)
SET(APPDIR CGenius)
SET(GAMES_SHAREDIR "${APPDIR}" CACHE PATH "Game data root dir")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
ELSE(WIN32)
SET(SHAREDIR "/usr/share" CACHE PATH "System share dir location")
SET(GAMES_SHAREDIR "${SHAREDIR}/games/" CACHE PATH "Game data root dir")
SET(DATADIR "${GAMES_SHAREDIR}/commandergenius")
SET(APPDIR games CACHE PATH "Binary destination")
SET(DOCDIR ${DATADIR} CACHE PATH "Docs destination")
SET(ICONDIR "${SHAREDIR}/icons/hicolor" CACHE PATH "Standard icon installation dir")
SET(DESKTOPFILESDIR "${SHAREDIR}/applications" CACHE PATH "Application installation dir")
ENDIF(WIN32)

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
endif(NINTENDO_SWITCH)


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

# Pass correct definition for SYSTEM_DATA_DIR so packager or distributors can decide where the app should land in
if(UNIX)
        if(NOT IS_ABSOLUTE ${GAMES_SHAREDIR})
                set(FULL_GAMES_SHAREDIR "${CMAKE_INSTALL_PREFIX}/${GAMES_SHAREDIR}" CACHE PATH "Full where the games will be stored")
        else()
                set(FULL_GAMES_SHAREDIR ${GAMES_SHAREDIR} CACHE PATH "Full where the games will be stored")
        endif()
endif(UNIX)

set(SYSTEM_DATA_DIR "${FULL_GAMES_SHAREDIR}" CACHE PATH "System data where CG will access to" )
MESSAGE(STATUS "Setting SYSTEM_DATA_DIR to ${SYSTEM_DATA_DIR}" )
add_definitions(-DSYSTEM_DATA_DIR="${SYSTEM_DATA_DIR}")


# Add GsKit
add_subdirectory("GsKit")

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

# Modules path located at GsKit/CMake
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/GsKit/CMake")

if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
      set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")
    elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
      set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++1z")
      set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++1z")
    endif()
endif()

#------------------------------------------------------------------------------
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

# Since shell script get_version is not processed properly on windows, we have to define manually here.
# TODO: Better system for defining the version
set(CG_VERSION "3.0.8")

MESSAGE(STATUS "Preparing the Build-System for Commander Genius" )
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE CG_VERSION)
string(REGEX REPLACE "[\r\n]" "" CG_VERSION "${CG_VERSION}")

# Generate the README file with the correct version string
configure_file(README.md ${CMAKE_BINARY_DIR}/README.md COPYONLY)
configure_file(README.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md COPYONLY)
configure_file(README.md ${CMAKE_CURRENT_SOURCE_DIR}/debian/README.md COPYONLY)
configure_file(GsKit/LICENSE ${CMAKE_BINARY_DIR}/LICENSE COPYONLY)

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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")


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

add_custom_target(${PROJECT_NAME} SOURCES changelog.txt README.md GsKit/LICENSE)

MESSAGE(STATUS "CG_VERSION = ${CG_VERSION}" )

INCLUDE(package.cmake)

MESSAGE( STATUS "Build system is prepared. To Build the project just type \"make\"" )
MESSAGE( STATUS "If you want to create the installation package just type \"make package\" after you build the project" )

