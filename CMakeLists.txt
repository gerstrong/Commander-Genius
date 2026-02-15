# CMake file for development of Commander Genius

cmake_minimum_required(VERSION 3.12)

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

# iOS-specific configuration
if (IOS)
    # Make sure all configurations output libraries in the same directory instead of in sub-directories per config
    # This allows easier linking from the Xcode iOS project
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/ios/libs)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/ios/libs)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/ios/libs)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/ios/libs)

    # Disable ImageIO for SDL2_image as it's not available for iOS, just OS X, and will fail at link time
    set(SDL2IMAGE_BACKEND_IMAGEIO 0)

    include(FetchContent)

    FetchContent_Declare(
      SDL
      URL            https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.10.zip
      SOURCE_DIR     ${CMAKE_BINARY_DIR}/external/SDL2
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_Declare(
      SDL_image
      URL            https://github.com/libsdl-org/SDL_image/archive/release-2.8.2.zip
      SOURCE_DIR     ${CMAKE_BINARY_DIR}/external/SDL2_image
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_Declare(
      SDL_mixer
      URL            https://github.com/libsdl-org/SDL_mixer/archive/refs/tags/release-2.8.1.zip
      SOURCE_DIR     ${CMAKE_BINARY_DIR}/external/SDL2_mixer
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    FetchContent_Declare(
      SDL_ttf
      URL            https://github.com/libsdl-org/SDL_ttf/archive/refs/tags/release-2.24.0.zip
      SOURCE_DIR     ${CMAKE_BINARY_DIR}/external/SDL2_ttf
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      PATCH_COMMAND sh external/download.sh
    )

    FetchContent_Declare(
      curl
      URL            https://github.com/curl/curl/releases/download/curl-8_11_1/curl-8.11.1.zip
      SOURCE_DIR     ${CMAKE_BINARY_DIR}/external/curl
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # Build all libraries as static for iOS
    set(BUILD_SHARED_LIBS OFF)
    
    # Force SDL2 to build as static only
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL2IMAGE_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SDL2TTF_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    
    set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
    set(SDL_TEST_ENABLED_BY_DEFAULT OFF)

    # SDL main is not needed for iOS
    set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "" FORCE)

    # Disable OPUS and other optional features
    set(SDL2MIXER_OPUS OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MOD_XMP OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MOD OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_FLUIDSYNTH OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "" FORCE)
    set(SDL2TTF_VENDORED ON CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_NATIVE OFF CACHE BOOL "" FORCE)
    set(SDL2MIXER_MIDI_TIMIDITY ON CACHE BOOL "" FORCE)

    # Make sure debug libs have the same name as release ones
    set(SDL_CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
    set(SDL2IMAGE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
    set(SDL2MIXER_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
    set(SDL2TTF_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
    
    # Configure curl for iOS
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TESTS ON CACHE BOOL "" FORCE)
    set(CURL_USE_SECTRANSP ON CACHE BOOL "" FORCE)  # Use iOS native SSL
    set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)   # Don't need OpenSSL with SecureTransport
    set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
    set(CURL_CA_BUNDLE "none" CACHE STRING "" FORCE)
    set(CURL_CA_PATH "none" CACHE STRING "" FORCE)
    set(HTTP_ONLY ON CACHE BOOL "" FORCE)  # Only need HTTP/HTTPS

    FetchContent_MakeAvailable(SDL SDL_image SDL_mixer SDL_ttf curl)

    # SDL2_ttf needs to download free type before it will
    # execute_process(COMMAND ${CMAKE_BINARY_DIR}/external/SDL2_ttf/external/download.sh)
    
    # Set SDL2 variables for find_package compatibility
    # This prevents GsKit from trying to find SDL2 via find_package
    set(SDL2_FOUND TRUE)
    # Use a single primary include directory for SDL2_INCLUDE_DIR to avoid issues with FindSDL2.cmake
    # The main header location is sufficient for most purposes
    set(SDL2_INCLUDE_DIR ${CMAKE_BINARY_DIR}/external/SDL2/include)
    set(SDL2_LIBRARY SDL2-static)
    set(SDL2IMAGE_INCLUDE_DIR ${CMAKE_BINARY_DIR}/external/SDL2_image/include)
    set(SDL2IMAGE_LIBRARY SDL2_image)
    set(SDL2MIXER_INCLUDE_DIR ${CMAKE_BINARY_DIR}/external/SDL2_mixer/include)
    set(SDL2MIXER_LIBRARY SDL2_mixer)
    set(SDL2_TTF_INCLUDE_DIR ${CMAKE_BINARY_DIR}/external/SDL2_ttf/include)
    set(SDL2_TTF_LIBRARY SDL2_ttf)

    # Set curl variables for find_package compatibility
    set(CURL_FOUND TRUE)
    set(CURL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/external/curl/include)
    set(CURL_LIBRARY libcurl)  # Singular - required by FindCURL.cmake
    set(CURL_LIBRARIES libcurl)  # Plural - for consistency
    set(CURL_VERSION_STRING "8.11.1")

    # Add additional SDL2 build include directories directly
    include_directories(
        ${CMAKE_BINARY_DIR}/_deps/sdl-build/include
        ${CMAKE_BINARY_DIR}/_deps/sdl-build/include-config-debug
        ${CMAKE_BINARY_DIR}/_deps/sdl-build/include-config-release
    )

    # Export these for subdirectories
    set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIR} CACHE STRING "SDL2 include directory")
    set(SDL2_LIBRARY ${SDL2_LIBRARY} CACHE STRING "SDL2 library")
    set(CURL_INCLUDE_DIR ${CURL_INCLUDE_DIR} CACHE STRING "CURL include directory")
    set(CURL_LIBRARY ${CURL_LIBRARY} CACHE STRING "CURL library")
    set(CURL_LIBRARIES ${CURL_LIBRARIES} CACHE STRING "CURL libraries")



endif()

option(DOWNLOADER "Internal Downloader" ON)

if (OSXCROSS)
    message(STATUS "OSXCross is enabled")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    include(${CMAKE_TOOLCHAIN_FILE})
endif()

option(USE_VIRTUALPAD "Enable Onscreen Virtual Gamepad support " ON)
option(EMBEDDED "Set this to yes, if you want a build for embedded systems" OFF)

if(ALTERNATE_HOME)
    ADD_DEFINITIONS(-DALTERNATE_HOME="${ALTERNATE_HOME}")
    MESSAGE(STATUS "ALTERNATE_HOME=${ALTERNATE_HOME}")
endif(ALTERNATE_HOME)

if(EMBEDDED)
    ADD_DEFINITIONS(-DEMBEDDED)
endif(EMBEDDED)

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
if(UNIX AND NOT IOS)
    if(NOT IS_ABSOLUTE ${GAMES_SHAREDIR})
        set(FULL_GAMES_SHAREDIR "${CMAKE_INSTALL_PREFIX}/${GAMES_SHAREDIR}" CACHE PATH "Full where the games will be stored")
    else()
        set(FULL_GAMES_SHAREDIR ${GAMES_SHAREDIR} CACHE PATH "Full where the games will be stored")
    endif()
    set(SYSTEM_DATA_DIR "${FULL_GAMES_SHAREDIR}" CACHE PATH "System data where CG will access to" )
elseif(IOS)
    # For iOS, data will be in the app bundle
    set(SYSTEM_DATA_DIR "." CACHE PATH "System data where CG will access to" )
endif()

MESSAGE(STATUS "Setting SYSTEM_DATA_DIR to ${SYSTEM_DATA_DIR}" )
add_definitions(-DSYSTEM_DATA_DIR="${SYSTEM_DATA_DIR}")

# For iOS, ensure SDL2 is available before adding GsKit
if(IOS)
    # Make SDL2 variables available to GsKit
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2IMAGE_INCLUDE_DIR})
    include_directories(${SDL2MIXER_INCLUDE_DIR})
    include_directories(${SDL2_TTF_INCLUDE_DIR})
endif()

# Add GsKit
add_subdirectory("GsKit")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/GsKit/CMake)

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER OR IOS)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

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
else()
  message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
endif()

# Version handling
set(CG_VERSION "3.6.2")

MESSAGE(STATUS "Preparing the Build-System for Commander Genius" )

# Only run shell script on Unix-like systems, not iOS
if(UNIX AND NOT IOS)
    execute_process(COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE CG_VERSION)
    string(REGEX REPLACE "[\r\n]" "" CG_VERSION "${CG_VERSION}")
endif()

# Generate the README file with the correct version string
configure_file(README.md ${CMAKE_BINARY_DIR}/README.md COPYONLY)
configure_file(README.md ${CMAKE_CURRENT_SOURCE_DIR}/README.md COPYONLY)
configure_file(README.md ${CMAKE_CURRENT_SOURCE_DIR}/debian/README.md COPYONLY)
configure_file(GsKit/LICENSE ${CMAKE_BINARY_DIR}/LICENSE COPYONLY)

if(APPLE AND NOT IOS)
    file(COPY cglogo.icns DESTINATION ${CMAKE_BINARY_DIR}/src)
    file(COPY Info.plist DESTINATION ${CMAKE_BINARY_DIR}/src)
    file(COPY launchCGonMacOs.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY buildMacOsBundle.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY makebundleable.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY get_version.sh DESTINATION ${CMAKE_BINARY_DIR})
    file(COPY version.h DESTINATION ${CMAKE_BINARY_DIR})
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -headerpad_max_install_names")
endif()

if(NOT IOS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
endif()

# Check that GsKit also has been loaded
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/GsKit/CMakeLists.txt")
    MESSAGE(FATAL_ERROR "You need to have the sources of GsKit in order to build Commander Genius. ")
endif()

# And build Commander Genius on top of that!
add_subdirectory("src")

add_custom_target(${PROJECT_NAME} SOURCES changelog.txt README.md)

MESSAGE(STATUS "CG_VERSION = ${CG_VERSION}" )

INCLUDE(package.cmake)

MESSAGE( STATUS "Build system is prepared. To Build the project just type \"make\"" )
MESSAGE( STATUS "If you want to create the installation package just type \"make package\" after you build the project" )

