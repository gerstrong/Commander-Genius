# specify properties of thehost system
# i.e. system you're compiling ON
set(CMAKE_HOST_SYSTEM Linux)
set(CMAKE_HOST_SYSTEM_PROCESSOR x86_64)

#set(CMAKE_SYSTEM_NAME "Darwin")

# specify properties of the target system
# i.e. system you're compiling FOR
set(CMAKE_SYSTEM_APPLE)
set(CMAKE_SYSTEM_VERSION 21.4)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# set osxcross directory as the root path for find(*)
# EDIT THIS IF YOU INSTALLED OSXCROSS ELSEWHERE (e.g. /opt/osxcross)
set(CMAKE_FIND_ROOT_PATH /tmp/osxcross)

# specify compilers to use
# C Compiler - clang 
set(CMAKE_C_COMPILER "/tmp/osxcross/target/bin/x86_64-apple-darwin21.4-clang")

# C++ compilers - clang++
set(CMAKE_CXX_COMPILER "/tmp/osxcross/target/bin/x86_64-apple-darwin21.4-clang++")
#set(PKG_CONFIG_EXECUTABLE x86_64-apple-darwin20.4-pkg-config)

# set search path options for compilers, libraries, and packages
# Search for programs in the build host directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set to build static libraries
#set(BUILD_SHARED_LIBS OFF)

set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")

