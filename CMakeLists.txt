# CMake file for development of OLX
# this file is not intendent (yet) as a system for building final releases
# use compile.sh instead

cmake_minimum_required(VERSION 2.4)
IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)
	cmake_policy(SET CMP0005 OLD)
	cmake_policy(SET CMP0003 OLD)
	#include(PCHSupport_26.cmake)
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.4)

SET(GP2X_WIZ 0)

OPTION(DEBUG "enable debug build" Yes)
OPTION(STLPORT "STLport support" No)
OPTION(GCOREDUMPER "Google Coredumper support" No)
#OPTION(PCH "Precompiled header (CMake 2.6 only)" No)
OPTION(ADVASSERT "Advanced assert" No)

OPTION(SDL "SDL support (probably will not work without)" Yes)
OPTION(OPENGL "OpenGL support" Yes)
OPTION(OGG "Ogg/Vorbis support" Yes)

IF (DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE (DEBUG)
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (DEBUG)

# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

IF(UNIX)
	IF (GP2X_WIZ)
		set(CMAKE_SYSTEM_NAME Linux)
		set(PREFIX /mythtv/media/devel/toolchains/openwiz/arm-openwiz-linux-gnu)
		set(CMAKE_C_COMPILER ${PREFIX}/bin/arm-openwiz-linux-gnu-gcc)
		set(CMAKE_CXX_COMPILER ${PREFIX}/bin/arm-openwiz-linux-gnu-g++)
		set(CMAKE_FIND_ROOT_PATH ${PREFIX})
		set(CMAKE_INSTALL_PREFIX ${PREFIX})
		set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
		set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		set(CMAKE_FIND_LIBRARY_PREFIXES lib)
		set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
		set(OPENGL No)
		set(CMAKE_VERBOSE_MAKEFILE ON)
		ADD_DEFINITIONS(-DWIZ)
		ADD_DEFINITIONS(-funsigned-char)
		include_directories(${PREFIX}/include)
		link_directories(${PREFIX}/lib)
	#ELSE(GP2X_WIZ)
	#	set(PREFIX /usr)
	#	set(CMAKE_C_COMPILER ${PREFIX}/bin/gcc)
	#	set(CMAKE_CXX_COMPILER ${PREFIX}/bin/g++)
	#	set(OPENGL No)
	#	set(CMAKE_VERBOSE_MAKEFILE ON)
	ENDIF(GP2X_WIZ)

	IF(APPLE)
	ELSE(APPLE)
	ENDIF(APPLE)

	IF (CYGWIN)
	ELSE (CYGWIN)
	ENDIF (CYGWIN)
ELSE(UNIX)
	IF(WIN32)
	ELSE(WIN32)
	ENDIF(WIN32)
ENDIF(UNIX)


MESSAGE( "DEBUG = ${DEBUG}" )
MESSAGE( "STLPORT = ${STLPORT}" )
MESSAGE( "GCOREDUMPER = ${GCOREDUMPER}" )
MESSAGE( "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}" )
MESSAGE( "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}" )
MESSAGE( "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
MESSAGE( "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
# commented out because only devs need these options anyway
#MESSAGE( "PCH = ${PCH} (Precompiled header, CMake 2.6 only)" )
#MESSAGE( "ADVASSERT = ${ADVASSERT}" )

MESSAGE( "OPENGL = ${OPENGL}" )
MESSAGE( "OGG = ${OGG}" )

EXEC_PROGRAM(mkdir ARGS -p ${CMAKE_CURRENT_SOURCE_DIR}/bin OUTPUT_VARIABLE NULL)

PROJECT(commandergenius)

# main includes
INCLUDE_DIRECTORIES(optional-includes/generated)
INCLUDE_DIRECTORIES(src)

IF(ADVASSERT)
	INCLUDE_DIRECTORIES(optional-includes/advanced-assert)
ENDIF(ADVASSERT)

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

SET(ALL_SRCS ${ROOT_S} ${AI_S} ${FILEIO_S} ${HQP_S} ${SCALE2X_S} ${SDL_S} ${SDLSOUND_S} ${SDLVIDEO_S} ${VORTICON_S} ${CINEMATICS_S} ${DIALOG_S})

IF (OGG)
	ADD_DEFINITIONS(-DBUILD_WITH_OGG)
	SET(ALL_SRCS ${ALL_SRCS} ${VORBIS_S})
ENDIF (OGG)

IF (SDL)
	ADD_DEFINITIONS(-DBUILD_SDL)
ENDIF (SDL)

IF (OPENGL)
	ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)

IF (GCOREDUMPER)
	INCLUDE_DIRECTORIES(libs/coredumper/src)
	ADD_DEFINITIONS(-DGCOREDUMPER)
	AUX_SOURCE_DIRECTORY(libs/coredumper/src COREDUMPER_SRCS)
	SET(ALL_SRCS ${ALL_SRCS} ${COREDUMPER_SRCS})
ENDIF (GCOREDUMPER)

IF (STLPORT)
	INCLUDE_DIRECTORIES(/usr/include/stlport)
	ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64) # hm, don't know, at least it works for me (ppc32/amd32)
# debugging stuff for STLport
	ADD_DEFINITIONS(-D_STLP_DEBUG=1)
	ADD_DEFINITIONS(-D_STLP_DEBUG_LEVEL=_STLP_STANDARD_DBG_LEVEL)
	ADD_DEFINITIONS(-D_STLP_SHRED_BYTE=0xA3)
	ADD_DEFINITIONS(-D_STLP_DEBUG_UNINITIALIZED=1)
	ADD_DEFINITIONS(-D_STLP_DEBUG_ALLOC=1)
ENDIF (STLPORT)


IF(DEBUG)
	ADD_DEFINITIONS(-DDEBUG=1)
ENDIF(DEBUG)



IF(WIN32)
#	EXEC_PROGRAM("${CMAKE_BINARY_DIR}/svn_revision.exe" ARGS "${CMAKE_SOURCE_DIR}/optional-includes/generated/Version_generated.h" "${CMAKE_SOURCE_DIR}")
#	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -DHAVE_BOOST -DZLIB_WIN32_NODLL)
	SET(OPTIMIZE_COMPILER_FLAG /Ox /Ob2 /Oi /Ot /GL)
	IF(DEBUG)
		ADD_DEFINITIONS(-DUSE_DEFAULT_MSC_DELEAKER)
	ELSE(DEBUG)
		ADD_DEFINITIONS(${OPTIMIZE_COMPILER_FLAG})
	ENDIF(DEBUG)
ELSE(WIN32)
	ADD_DEFINITIONS(-Wall)

#	EXEC_PROGRAM(sh ARGS ${CMAKE_CURRENT_SOURCE_DIR}/get_version.sh OUTPUT_VARIABLE OLXVER)
#	string(REGEX REPLACE "[\r\n]" " " OLXVER "${OLXVER}")
#	MESSAGE( "OLX_VERSION = ${OLXVER}" )
	# Escaping backslashes overkill, yay.
#	EXEC_PROGRAM(sh ARGS -c echo\\ \\\\\#define\\ LX_VERSION\\ \\\\\\"${OLXVER}\\\\\\" > optional-includes/generated/Version_generated.h OUTPUT_VARIABLE DUMMY)

	IF(SDL)
		EXEC_PROGRAM(${PREFIX}/bin/sdl-config ARGS --cflags OUTPUT_VARIABLE SDLCFLAGS)
		string(REGEX REPLACE "[\r\n]" " " SDLCFLAGS "${SDLCFLAGS}")
		ADD_DEFINITIONS(${SDLCFLAGS})
	ENDIF(SDL)

	ADD_DEFINITIONS("-pthread")
	SET(OPTIMIZE_COMPILER_FLAG -O3)
ENDIF(WIN32)

ADD_EXECUTABLE(commandergenius ${ALL_SRCS})

#IF(PCH)
#	EXEC_PROGRAM(./update_precompiled_header.sh OUTPUT_VARIABLE NULL)
#	ADD_PRECOMPILED_HEADER(openlierox include/PrecompiledHeader.hpp 1)
#ENDIF(PCH)

IF(SDL)
	TARGET_LINK_LIBRARIES(commandergenius SDL)
ENDIF(SDL)

IF(WIN32)
	TARGET_LINK_LIBRARIES(commandergenius wsock32 wininet dbghelp
				"${CMAKE_SOURCE_DIR}/build/msvc/libs/SDLmain.lib"
				"${CMAKE_SOURCE_DIR}/build/msvc/libs/bgd.lib")

	SET_TARGET_PROPERTIES(commandergenius PROPERTIES OUTPUT_NAME "../../../bin/Commander Genius")
	
	# Add debug info to release build in linker flags
	SET_TARGET_PROPERTIES(openlierox PROPERTIES LINK_FLAGS "${LINK_FLAGS} 
		/LTCG
		/DEBUG
		/MAP:\"${CMAKE_SOURCE_DIR}/bin/CommanderGenius.map\"
		/MAPINFO:EXPORTS
		/NODEFAULTLIB:\"LIBC\"
		/INCREMENTAL:NO")
ELSE(WIN32)
	IF(SDL)
		EXEC_PROGRAM(${PREFIX}/bin/sdl-config ARGS --libs OUTPUT_VARIABLE SDLLIBS)
		STRING(REGEX REPLACE "[\r\n]" " " SDLLIBS "${SDLLIBS}")

		TARGET_LINK_LIBRARIES(commandergenius ${SDLLIBS})
	ENDIF(SDL)

	IF(OGG)
		TARGET_LINK_LIBRARIES(commandergenius ogg vorbis vorbisfile)
	ENDIF(OGG)

	IF (STLPORT)
		TARGET_LINK_LIBRARIES(commandergenius stlportstlg)
	ENDIF (STLPORT)

	SET_TARGET_PROPERTIES(commandergenius PROPERTIES OUTPUT_NAME bin/commandergenius)

ENDIF(WIN32)
