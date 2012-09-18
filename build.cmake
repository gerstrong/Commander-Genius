

PROJECT(CommanderGenius)

OPTION(OPENGL "OpenGL support" Yes)
OPTION(OGG "Ogg/Vorbis support" Yes)
OPTION(TREMOR "Tremor support" No)


#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()



IF(NOT DEFINED BUILD_TYPE)
	SET(BUILD_TYPE LINUX)
	MESSAGE( "WARNING: You did not specify the build type with the -DBUILD_TYPE= Parameter. LINUX has been choosen" )
	MESSAGE( "Available builds are: WIN32, LINUX GP2X. WIZ, CAANOO, DINGOO, NANONOTE" )
ENDIF(NOT DEFINED BUILD_TYPE)


# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

SET(USRDIR /usr)

# Compilation Flags and executables
IF (BUILD_TYPE STREQUAL GP2X OR
    BUILD_TYPE STREQUAL WIZ OR
    BUILD_TYPE STREQUAL CAANOO OR
    BUILD_TYPE STREQUAL DINGOO OR
    BUILD_TYPE STREQUAL PANDORA OR
    BUILD_TYPE STREQUAL NANONOTE)
	IF (BUILD_TYPE STREQUAL GP2X)
		SET(USRDIR /mythtv/media/devel/toolchains/open2x/gcc-4.1.1-glibc-2.3.6)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-open2x-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-open2x-linux-g++)
		ADD_DEFINITIONS(-DGP2X)
		ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG Yes)
	ELSEIF(BUILD_TYPE STREQUAL WIZ)
		SET(USRDIR /mythtv/media/devel/toolchains/openwiz/arm-openwiz-linux-gnu)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-openwiz-linux-gnu-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-openwiz-linux-gnu-g++)
		ADD_DEFINITIONS(-DWIZ)
                ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG Yes)
        ELSEIF(BUILD_TYPE STREQUAL CAANOO)
                SET(USRDIR /mythtv/media/devel/toolchains/caanoo/GPH_SDK)
                SET(CMAKE_C_COMPILER ${USRDIR}/tools/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-gcc)
                SET(CMAKE_CXX_COMPILER ${USRDIR}/tools/gcc-4.2.4-glibc-2.7-eabi/bin/arm-gph-linux-gnueabi-g++)
                ADD_DEFINITIONS(-DCAANOO)
                SET(OGG Yes)

                include_directories(${USRDIR}/DGE/include)
                include_directories(${USRDIR}/DGE/include/vorbis)
                link_directories(${USRDIR}/DGE/lib/target)
                ADD_DEFINITIONS(-DCAANOO)
                ADD_DEFINITIONS(-DNOKEYBOARD)
                SET(OGG Yes)
	ELSEIF(BUILD_TYPE STREQUAL DINGOO)
		SET(USRDIR /opt/opendingux-toolchain/usr/)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/mipsel-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/mipsel-linux-g++)
                include_directories(${USRDIR}/include/tremor)
		ADD_DEFINITIONS(-DDINGOO)
                ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG No)
		SET(TREMOR Yes)
	ELSEIF(BUILD_TYPE STREQUAL PANDORA)
		SET(USRDIR /mythtv/media/devel/toolchains/pandora/arm-2007q3)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-none-linux-gnueabi-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-none-linux-gnueabi-g++)
		ADD_DEFINITIONS(-DPANDORA)
                ADD_DEFINITIONS(-mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a)
                ADD_DEFINITIONS(-mfloat-abi=softfp -mfpu=neon -ftree-vectorize -fno-strict-aliasing -fsingle-precision-constant)

		SET(OGG Yes)
	ELSEIF(BUILD_TYPE STREQUAL NANONOTE)
		SET(USRDIR /usr/local/jlime/mipsel-toolchain/usr)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/mipsel-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/mipsel-linux-g++)
		ADD_DEFINITIONS(-DNANONOTE)
		SET(OGG No)
		SET(TREMOR No)
	ENDIF(BUILD_TYPE STREQUAL GP2X)

	SET(BUILD_TYPE LINUX32)
	SET(CMAKE_SYSTEM_NAME Linux)
	SET(CMAKE_FIND_ROOT_PATH ${USRDIR})
	SET(CMAKE_INSTALL_USRDIR ${USRDIR})
	SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	SET(CMAKE_FIND_LIBRARY_USRDIRES lib)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES .a)
	SET(OPENGL No)
	SET(CMAKE_VERBOSE_MAKEFILE ON)

	ADD_DEFINITIONS(-fsigned-char)
	ADD_DEFINITIONS(-ffast-math)
	ADD_DEFINITIONS(-fomit-frame-pointer)
        IF (NOT BUILD_TYPE STREQUAL CAANOO)
                include_directories(${USRDIR}/include)
                include_directories(${USRDIR}/include/vorbis)
                link_directories(${USRDIR}/lib)
        ENDIF (NOT BUILD_TYPE STREQUAL CAANOO)
ENDIF (BUILD_TYPE STREQUAL GP2X OR
    BUILD_TYPE STREQUAL WIZ OR
    BUILD_TYPE STREQUAL CAANOO OR
    BUILD_TYPE STREQUAL DINGOO OR
    BUILD_TYPE STREQUAL PANDORA OR
    BUILD_TYPE STREQUAL NANONOTE)


# Directory creation of all the chosen build 
set(BUILD_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build")
set(BUILD_DIR "${BUILD_BASE_DIR}/${BUILD_TYPE}")
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_BASE_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})

# Generate the README file
configure_file(README.in README)

# main includes
INCLUDE_DIRECTORIES(src)

file(GLOB_RECURSE ALL_SRCS src/*.c*)

IF(UNIX)
# Compilation under Linux

#	IF(BUILD_TYPE STREQUAL WIN32)
#		option(MINGW "Use MingW" yes)		
#		set(MINGW_PREFIX "i686-w64-mingw32")
#		set(PREFIX /usr/${MINGW_PREFIX})		
#		set(CMAKE_PREFIX_PATH ${PREFIX})
#		set(CMAKE_C_COMPILER ${MINGW_PREFIX}-gcc)
#		set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}-g++)
#		set(CMAKE_INCLUDE_PATH ${PREFIX}/include)
#		set(CMAKE_FIND_ROOT_PATH ${PREFIX})
#		set(CMAKE_INSTALL_PREFIX ${PREFIX})
#		set(CMAKE_LIBRARY_PATH ${PREFIX}/lib)	
#		ADD_DEFINITIONS(-DWIN32)
#		ADD_DEFINITIONS(-DTARGET_WIN32)
		
#	ELSEIF(BUILD_TYPE STREQUAL LINUX64)
		ADD_DEFINITIONS("-DTARGET_LNX")
#	ELSE(BUILD_TYPE STREQUAL WIN32)
#	ENDIF(BUILD_TYPE STREQUAL WIN32)
	
#	SET(OPTIMIZE_COMPILER_FLAG -O3)
ENDIF(UNIX)

find_package(PkgConfig)
pkg_check_modules(SDL sdl)


#temporary fix for the mingw compilation process
#if(BUILD_TYPE STREQUAL WIN32)
#	set(SDL_INCLUDE_DIRS ${PREFIX}/include/SDL)
#endif(BUILD_TYPE STREQUAL WIN32)
	
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})


if(OPENGL)
  find_package(OpenGL)
   if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIR})
  else (OPENGL_FOUND)
    MESSAGE("OpenGL environment missing")
  endif (OPENGL_FOUND)
endif()


# Try to use Ogg if enabled
IF (OGG)
    FIND_PATH(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h DOC "Include for Ogg Vorbis")
    SET(VORBIS_INCLUDE_PATH "${VORBIS_INCLUDE_DIR}/vorbis")
	INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_PATH})
	ADD_DEFINITIONS(-DOGG)
ENDIF (OGG)

IF (TREMOR)
	IF(BUILD_TYPE STREQUAL DINGOO)
		INCLUDE_DIRECTORIES(${USRDIR}/include/tremor)
	ELSE (BUILD_TYPE STREQUAL DINGOO)
		INCLUDE_DIRECTORIES(${CMAKE_INCLUDE_PATH}/tremor)
	ENDIF (BUILD_TYPE STREQUAL DINGOO)
	ADD_DEFINITIONS(-DTREMOR)
ENDIF (TREMOR)



IF (OPENGL)
	ADD_DEFINITIONS(-DGL)
	ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)


# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DDEBUG)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-DRELEASE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Executable definition
ADD_EXECUTABLE(CommanderGenius ${ALL_SRCS})

# Linking part under Linux
IF(BUILD_TYPE STREQUAL WIN32)
	TARGET_LINK_LIBRARIES(CommanderGenius mingw32)
	TARGET_LINK_LIBRARIES(CommanderGenius SDLmain)
	TARGET_LINK_LIBRARIES(CommanderGenius SDL)
	TARGET_LINK_LIBRARIES(CommanderGenius glu32)
	TARGET_LINK_LIBRARIES(CommanderGenius opengl32)
ENDIF(BUILD_TYPE STREQUAL WIN32)


IF(OPENGL)
	TARGET_LINK_LIBRARIES(CommanderGenius GL)
ENDIF(OPENGL)

IF(OGG)
	TARGET_LINK_LIBRARIES(CommanderGenius vorbis vorbisfile)
ENDIF(OGG)

IF(TREMOR)
	TARGET_LINK_LIBRARIES(CommanderGenius vorbisidec)
ENDIF(TREMOR)

# Set binary(executable) file name. In Windows this filename needs the exe extension
# The Windows Version has an icon in the executable


#IF(BUILD_TYPE STREQUAL WIN32)
#	SET_TARGET_PROPERTIES(commandergenius PROPERTIES OUTPUT_NAME "${BUILD_DIR}/CGenius.exe")
#	ADD_CUSTOM_COMMAND(TARGET commandergenius PRE_LINK COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc )
#	ADD_CUSTOM_COMMAND(TARGET commandergenius PRE_LINK COMMAND icotool -c -o ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.ico ${CMAKE_CURRENT_SOURCE_DIR}/vfsroot/cglogo512.png )
#	ADD_CUSTOM_COMMAND(TARGET commandergenius PRE_LINK COMMAND ${MINGW_PREFIX}-windres ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o)
#	SET_TARGET_PROPERTIES(commandergenius PROPERTIES LINK_FLAGS "${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o")
#	MESSAGE( "Will build with the CG-Icon for Windows." )
#ELSE(BUILD_TYPE STREQUAL WIN32)
#	SET_TARGET_PROPERTIES(commandergenius PROPERTIES OUTPUT_NAME "build/${BUILD_DIR}/CGenius")
#ENDIF(BUILD_TYPE STREQUAL WIN32)

target_link_libraries(CommanderGenius ${SDL_LIBRARIES})


#MESSAGE( "BUILD_TYPE = ${BUILD_TYPE}" )
#MESSAGE( "BUILD_DIR = ${BUILD_DIR}" )
#MESSAGE( "DEBUG = ${DEBUG}" )
#MESSAGE( "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}" )
#MESSAGE( "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}" )
#MESSAGE( "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
#MESSAGE( "CMAKE_FIND_ROOT_PATH = ${CMAKE_FIND_ROOT_PATH}" )
#MESSAGE( "CMAKE_INCLUDE_PATH = ${CMAKE_INCLUDE_PATH}" )
#MESSAGE( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
#MESSAGE( "CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}" )
#MESSAGE( "SDL_INCLUDE_DIRS = ${SDL_INCLUDE_DIRS}" )
#MESSAGE( "SDL_LIBRARIES = ${SDL_LIBRARIES}" )

#MESSAGE( "Compile it with OpenGL : OPENGL = ${OPENGL}" )
#MESSAGE( "We use OGG Vorbis : OGG = ${OGG}" )
#MESSAGE( "We use Tremor : TREMOR = ${TREMOR}" )

#MESSAGE( "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
#MESSAGE( "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}" )
#MESSAGE( "You are on 64-bit System : HAVE_64_BIT = ${HAVE_64_BIT}" )
#MESSAGE( "USRDIR = ${USRDIR}" )
#MESSAGE( "Commander Genius Version : CG_VERSION = ${CG_VERSION}" )
#MESSAGE("VORBIS_INCLUDE_PATH = ${VORBIS_INCLUDE_PATH}")
