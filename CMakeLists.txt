# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 3.1)


OPTION(OGG "Ogg/Vorbis support" Yes)
OPTION(TREMOR "Tremor support" No)
OPTION(DOWNLOADER "Internal Downloader" Yes)
OPTION(USE_PYTHON3 "Python3 for scripting" No)
OPTION(USE_SDL2 "SDL2 support" Yes)


Project(CommanderGenius)

if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
#include(cotire)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

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




# Force SDL2 on windows systems
IF(WIN32)

    INCLUDE(FindPkgConfig)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED >=2.0.0)

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIR})
        include_directories(${SDL2_NET_INCLUDE_DIRS})
        include_directories(/usr/include/SDL2/)
    endif(SDL2_FOUND)

	
else(WIN32)
        if(USE_SDL2)
 
                INCLUDE(FindPkgConfig)
                find_package(SDL2 REQUIRED)
                find_package(SDL2_image REQUIRED >=2.0.0)
   

                set(SDL2_INCLUDE_DIRS /Library/Frameworks/SDL2.framework/Headers)
                set(SDL2_IMAGE_INCLUDE_DIRS /Library/Frameworks/SDL2_image.framework/Headers)

        else(USE_SDL2)

            # SDL2 not defined, try SDL1.2
            FIND_PACKAGE( SDL REQUIRED )
            FIND_PACKAGE( SDL_net REQUIRED )
            if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                include(FindSDL OPTIONAL)
            endif()

            if(SDL_FOUND)
                message(STATUS "Using shared SDL Version 1.2")
                include_directories(${SDL_INCLUDE_DIR})
                INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})
            else(SDL_FOUND)
                # TODO: Use the prebuilt one on Windows
                message("Using static SDL from Externals")
                include_directories(Externals/SDL Externals/SDL/include)
                add_subdirectory(Externals/SDL)
            endif(SDL_FOUND)

        endif(USE_SDL2)

endif(WIN32)


# Add GsKit
add_subdirectory("GsKit")


# Compile to internal Commander Genius sources
add_subdirectory("src")

INCLUDE(package.cmake)
