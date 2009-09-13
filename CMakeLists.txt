# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. If someone wants 
# to adapt windows, he is welcome, to do so.

cmake_minimum_required(VERSION 2.4)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
	cmake_policy(SET CMP0005 OLD)
	cmake_policy(SET CMP0003 OLD)
	#include(PCHSupport_26.cmake)
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)

#if the use did not write -DBUILD_TYPE=WIN32 for example
IF(DEFINED BUILD_TYPE)
	MESSAGE( "BUILD_TYPE = ${BUILD_TYPE}" )
ELSE(DEFINED BUILD_TYPE)
	MESSAGE( "BUILD_TYPE = not defined" )
	SET(BUILD_TYPE LINUX)
	MESSAGE( "WARNING: You did not specify the build type with the -DBUILD_TYPE= Parameter. LINUX has been choosen" )
	MESSAGE( "Avialable Builds are: WIN32, LINUX, GP2X. For 64-bit Linux use -DLINUX64=yes" )
ENDIF(DEFINED BUILD_TYPE)

OPTION(DEBUG "enable debug build" No)
OPTION(SDL "SDL support (will not work without)" Yes)
OPTION(OPENGL "OpenGL support" Yes)
OPTION(OGG "Ogg/Vorbis support" Yes)
OPTION(LINUX64 "Linux 64-bit Build" No)

IF (DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE (DEBUG)
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (DEBUG)

# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

# Compilation Flags
IF(UNIX)
	IF (BUILD_TYPE EQUAL GP2X)
	set(CMAKE_SYSTEM_NAME Linux)
		IF (GP2X_ONLY)
			set(PREFIX /mythtv/media/devel/toolchains/open2x/gcc-4.1.1-glibc-2.3.6)
			set(CMAKE_C_COMPILER ${PREFIX}/bin/arm-open2x-linux-gcc)
			set(CMAKE_CXX_COMPILER ${PREFIX}/bin/arm-open2x-linux-g++)
			ADD_DEFINITIONS(-DGP2X)
		ELSE (GP2X_ONLY)
			set(PREFIX /mythtv/media/devel/toolchains/openwiz/arm-openwiz-linux-gnu)
			set(CMAKE_C_COMPILER ${PREFIX}/bin/arm-openwiz-linux-gnu-gcc)
			set(CMAKE_CXX_COMPILER ${PREFIX}/bin/arm-openwiz-linux-gnu-g++)
		ENDIF(GP2X_ONLY)
	set(CMAKE_FIND_ROOT_PATH ${PREFIX})
	set(CMAKE_INSTALL_PREFIX ${PREFIX})
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	set(CMAKE_FIND_LIBRARY_PREFIXES lib)
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	set(OPENGL No)
	set(OGG No)
	set(CMAKE_VERBOSE_MAKEFILE ON)
	ADD_DEFINITIONS(-DWIZGP2X)
	ADD_DEFINITIONS(-fsigned-char)
	ADD_DEFINITIONS(-ffast-math)
	ADD_DEFINITIONS(-fomit-frame-pointer)
	include_directories(${PREFIX}/include)
	link_directories(${PREFIX}/lib)
	ELSE(BUILD_TYPE EQUAL GP2X)
	ENDIF(BUILD_TYPE EQUAL GP2X)
ELSE(UNIX)
ENDIF(UNIX)

MESSAGE( "DEBUG = ${DEBUG}" )
MESSAGE( "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}" )
MESSAGE( "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}" )
MESSAGE( "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
MESSAGE( "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
MESSAGE( "OPENGL = ${OPENGL}" )
MESSAGE( "OGG = ${OGG}" )
MESSAGE( "LINUX64 = ${LINUX64}" )

# Directory creation of all the builds
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build OUTPUT_VARIABLE NULL)
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build/Linux32 OUTPUT_VARIABLE NULL)
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build/Linux64 OUTPUT_VARIABLE NULL)
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build/Windows OUTPUT_VARIABLE NULL)
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build/Wiz OUTPUT_VARIABLE NULL)
EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/build/Xcode OUTPUT_VARIABLE NULL)

PROJECT(commandergenius)

# main includes
INCLUDE_DIRECTORIES(optional-includes/generated)
INCLUDE_DIRECTORIES(src)

AUX_SOURCE_DIRECTORY(src ROOT_S)
AUX_SOURCE_DIRECTORY(src/ai AI_S)
AUX_SOURCE_DIRECTORY(src/fileio FILEIO_S)
AUX_SOURCE_DIRECTORY(src/hqp HQP_S)
AUX_SOURCE_DIRECTORY(src/scale2x SCALE2X_S)
AUX_SOURCE_DIRECTORY(src/sdl SDL_S)
AUX_SOURCE_DIRECTORY(src/sdl/sound SDLSOUND_S)
AUX_SOURCE_DIRECTORY(src/sdl/video SDLVIDEO_S)
AUX_SOURCE_DIRECTORY(src/vorbis VORBIS_S)
AUX_SOURCE_DIRECTORY(src/vorticon VORTICON_S)
AUX_SOURCE_DIRECTORY(src/cinematics CINEMATICS_S)
AUX_SOURCE_DIRECTORY(src/dialog DIALOG_S)
AUX_SOURCE_DIRECTORY(src/common COMMON_S)
AUX_SOURCE_DIRECTORY(src/graphics GRAPHICS_S)

SET(ALL_SRCS ${ROOT_S} ${AI_S} ${FILEIO_S} ${HQP_S} ${SCALE2X_S} ${SDL_S} ${SDLSOUND_S} ${SDLVIDEO_S} ${VORTICON_S} ${CINEMATICS_S} ${DIALOG_S} ${COMMON_S} ${GRAPHICS_S})

IF (OGG)
	INCLUDE_DIRECTORIES(/usr/include/vorbis)
	ADD_DEFINITIONS(-DBUILD_WITH_OGG)
	SET(ALL_SRCS ${ALL_SRCS} ${VORBIS_S})
ENDIF (OGG)

IF (SDL)
	INCLUDE_DIRECTORIES(/usr/include/SDL)
	ADD_DEFINITIONS(-DBUILD_SDL)
ENDIF (SDL)

IF (OPENGL)
	ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)

IF(DEBUG)
	ADD_DEFINITIONS(-DDEBUG)
ENDIF(DEBUG)

IF(UNIX)
# Compilation under Linux
	ADD_DEFINITIONS("-DTARGET_LNX -c -fmessage-length=0")

	IF(LINUX64)
		SET(BUILD_DIR Linux64)
	ELSE(LINUX64)
		ADD_DEFINITIONS(-D__i386__)
		ADD_DEFINITIONS(-m32)
		SET(BUILD_DIR Linux32)
	ENDIF(LINUX64)

	SET(OPTIMIZE_COMPILER_FLAG -O3)
ENDIF(UNIX)

# Executable definition
ADD_EXECUTABLE(commandergenius ${ALL_SRCS})

# Linking part under Linux
IF(SDL)
	IF(LINUX64)
		TARGET_LINK_LIBRARIES(commandergenius SDL)
	ELSE(LINUX64)
		TARGET_LINK_LIBRARIES(commandergenius SDL-1.2)
		SET_TARGET_PROPERTIES(commandergenius PROPERTIES LINK_FLAGS -L/usr/lib32)
		SET_TARGET_PROPERTIES(commandergenius PROPERTIES LINK_FLAGS -m32)
	ENDIF(LINUX64)
ENDIF(SDL)

IF(OPENGL)
	TARGET_LINK_LIBRARIES(commandergenius GL)
ENDIF(OPENGL)

IF(OGG)
	TARGET_LINK_LIBRARIES(commandergenius vorbisfile vorbis)
ENDIF(OGG)

MESSAGE( "BUILD_DIR = ${BUILD_DIR}" )

SET_TARGET_PROPERTIES(commandergenius PROPERTIES OUTPUT_NAME "build/${BUILD_DIR}/CGenius")

