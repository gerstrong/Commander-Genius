# specify properties of thehost system
# i.e. system you're compiling ON
set(CMAKE_HOST_SYSTEM Linux)
set(CMAKE_HOST_SYSTEM_PROCESSOR x86_64)


# 1. Determine the version
# Priorities: Command line (-DDARWIN_VERSION) > Environment Variable ($ENV{DARWIN_VERSION}) > Default (24.4)
if(NOT DARWIN_VERSION)
    if(DEFINED ENV{DARWIN_VERSION})
        set(DARWIN_VERSION $ENV{DARWIN_VERSION})
    else()
        set(DARWIN_VERSION "24.4")
    endif()
endif()

#set(CMAKE_SYSTEM_NAME "Darwin")

# specify properties of the target system
# i.e. system you're compiling FOR
set(CMAKE_SYSTEM_APPLE)
set(CMAKE_SYSTEM_VERSION ${DARWIN_VERSION})
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# set osxcross directory as the root path for find(*)
# EDIT THIS IF YOU INSTALLED OSXCROSS ELSEWHERE (e.g. /opt/osxcross)
set(CMAKE_FIND_ROOT_PATH /tmp/osxcross)

# specify compilers to use
# C Compiler - clang 
set(CMAKE_C_COMPILER "/tmp/osxcross/target/bin/x86_64-apple-darwin${DARWIN_VERSION}-clang")

# C++ compilers - clang++
set(CMAKE_CXX_COMPILER "/tmp/osxcross/target/bin/x86_64-apple-darwin${DARWIN_VERSION}-clang++")
message(STATUS "Cross-compiling for Darwin Version: ${DARWIN_VERSION}")

# set search path options for compilers, libraries, and packages
# Search for programs in the build host directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set to build static libraries
#set(BUILD_SHARED_LIBS OFF)

set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")

