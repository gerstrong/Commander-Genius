

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



IF(NOT DEFINED BUILD_TARGET)
	SET(BUILD_TARGET LINUX)
	MESSAGE( "WARNING: You did not specify the build type with the -DBUILD_TARGET= Parameter. LINUX has been choosen" )
	MESSAGE( "Available builds are: WIN32, LINUX GP2X. WIZ, CAANOO, DINGOO, NANONOTE" )
ENDIF(NOT DEFINED BUILD_TARGET)


# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

SET(USRDIR /usr)

# Compilation Flags and executables
IF (BUILD_TARGET STREQUAL GP2X OR
    BUILD_TARGET STREQUAL WIZ OR
    BUILD_TARGET STREQUAL CAANOO OR
    BUILD_TARGET STREQUAL DINGOO OR
    BUILD_TARGET STREQUAL PANDORA OR
    BUILD_TARGET STREQUAL NANONOTE)
	IF (BUILD_TARGET STREQUAL GP2X)
		SET(USRDIR /mythtv/media/devel/toolchains/open2x/gcc-4.1.1-glibc-2.3.6)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-open2x-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-open2x-linux-g++)
		ADD_DEFINITIONS(-DGP2X)
		ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG Yes)
	ELSEIF(BUILD_TARGET STREQUAL WIZ)
		SET(USRDIR /mythtv/media/devel/toolchains/openwiz/arm-openwiz-linux-gnu)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-openwiz-linux-gnu-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-openwiz-linux-gnu-g++)
		ADD_DEFINITIONS(-DWIZ)
                ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG Yes)
        ELSEIF(BUILD_TARGET STREQUAL CAANOO)
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
	ELSEIF(BUILD_TARGET STREQUAL DINGOO)
		SET(USRDIR /opt/opendingux-toolchain/usr/)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/mipsel-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/mipsel-linux-g++)
                include_directories(${USRDIR}/include/tremor)
		ADD_DEFINITIONS(-DDINGOO)
                ADD_DEFINITIONS(-DNOKEYBOARD)
		SET(OGG No)
		SET(TREMOR Yes)
	ELSEIF(BUILD_TARGET STREQUAL PANDORA)
		SET(USRDIR /mythtv/media/devel/toolchains/pandora/arm-2007q3)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/arm-none-linux-gnueabi-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/arm-none-linux-gnueabi-g++)
		ADD_DEFINITIONS(-DPANDORA)
                ADD_DEFINITIONS(-mcpu=cortex-a8 -mtune=cortex-a8 -march=armv7-a)
                ADD_DEFINITIONS(-mfloat-abi=softfp -mfpu=neon -ftree-vectorize -fno-strict-aliasing -fsingle-precision-constant)

		SET(OGG Yes)
	ELSEIF(BUILD_TARGET STREQUAL NANONOTE)
		SET(USRDIR /usr/local/jlime/mipsel-toolchain/usr)
		SET(CMAKE_C_COMPILER ${USRDIR}/bin/mipsel-linux-gcc)
		SET(CMAKE_CXX_COMPILER ${USRDIR}/bin/mipsel-linux-g++)
		ADD_DEFINITIONS(-DNANONOTE)
		SET(OGG No)
		SET(TREMOR No)
	ENDIF(BUILD_TARGET STREQUAL GP2X)

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
        IF (NOT BUILD_TARGET STREQUAL CAANOO)
                include_directories(${USRDIR}/include)
                include_directories(${USRDIR}/include/vorbis)
                link_directories(${USRDIR}/lib)
        ENDIF (NOT BUILD_TARGET STREQUAL CAANOO)
ENDIF ()



IF(BUILD_TARGET STREQUAL WIN32)
	ADD_DEFINITIONS(-std=c++0x)
else()
	ADD_DEFINITIONS(-std=c++0x)
endif()



# Directory creation of all the chosen build 
set(BUILD_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build")
set(BUILD_DIR "${BUILD_BASE_DIR}/${BUILD_TARGET}")
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
	IF(BUILD_TARGET STREQUAL WIN32)
	
		set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
		set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
	
		# the name of the target operating system
		SET(CMAKE_SYSTEM_NAME Windows)
		option(MINGW "Use MingW" yes)		
		set(MINGW_PREFIX "mingw32")
		set(PREFIX /opt/${MINGW_PREFIX})		
		set(CMAKE_PREFIX_PATH ${PREFIX})
		set(CMAKE_C_COMPILER /usr/bin/i686-w64-mingw32-gcc)
		set(CMAKE_CXX_COMPILER /usr/bin/i686-w64-mingw32-g++)
		set(CMAKE_INCLUDE_PATH ${PREFIX}/include)
		set(CMAKE_FIND_ROOT_PATH ${PREFIX})
		set(CMAKE_INSTALL_PREFIX ${PREFIX})
		set(CMAKE_LIBRARY_PATH ${PREFIX}/lib)	
		ADD_DEFINITIONS(-DWIN32)
		ADD_DEFINITIONS(-DTARGET_WIN32)
		
	ELSEIF(BUILD_TARGET STREQUAL LINUX)
		ADD_DEFINITIONS("-DTARGET_LNX")
	ELSE(BUILD_TARGET STREQUAL WIN32)
	ENDIF(BUILD_TARGET STREQUAL WIN32)
	
ENDIF(UNIX)



IF(BUILD_TARGET STREQUAL WIN32)
    set(SDL_INCLUDE_DIRS ${PREFIX}/include/SDL)
    include_directories(${SDL_INCLUDE_DIRS})
ELSE(BUILD_TARGET STREQUAL WIN32)
    find_package(PkgConfig)
    pkg_check_modules(SDL sdl)
ENDIF(BUILD_TARGET STREQUAL WIN32)

Find_Package ( SDL_image REQUIRED )



#temporary fix for the mingw compilation process
#if(BUILD_TARGET STREQUAL WIN32)
#	set(SDL_INCLUDE_DIRS ${PREFIX}/include/SDL)
#endif(BUILD_TARGET STREQUAL WIN32)
	
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})


if(OPENGL)
	IF(BUILD_TARGET STREQUAL WIN32)
		set(OPENGL_INCLUDE_DIR ${CMAKE_INCLUDE_PATH}/GL)	
		include_directories(${OPENGL_INCLUDE_DIR})
	ELSE(BUILD_TARGET STREQUAL WIN32)
	  find_package(OpenGL)
	    if (OPENGL_FOUND)
		include_directories(${OPENGL_INCLUDE_DIR})
	    else (OPENGL_FOUND)
		MESSAGE("OpenGL environment missing")
	    endif()
	ENDIF()
endif()


# Try to use Ogg if enabled
IF (OGG)
    IF(BUILD_TARGET STREQUAL WIN32)
	set(VORBIS_INCLUDE_DIR ${CMAKE_INCLUDE_PATH})
    ELSE(BUILD_TARGET STREQUAL WIN32)
	FIND_PATH(VORBIS_INCLUDE_DIR vorbis/vorbisfile.h DOC "Include for Ogg Vorbis")	
    ENDIF()
    
    SET(VORBIS_INCLUDE_PATH "${VORBIS_INCLUDE_DIR}/vorbis")	   
    ADD_DEFINITIONS(-DOGG)
    INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_PATH})
    INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_DIR})
ENDIF (OGG)

IF (TREMOR)
	IF(BUILD_TARGET STREQUAL DINGOO)
		INCLUDE_DIRECTORIES(${USRDIR}/include/tremor)
	ELSE (BUILD_TARGET STREQUAL DINGOO)
		INCLUDE_DIRECTORIES(${CMAKE_INCLUDE_PATH}/tremor)
	ENDIF (BUILD_TARGET STREQUAL DINGOO)
	ADD_DEFINITIONS(-DTREMOR)
ENDIF (TREMOR)




IF (OPENGL)
	ADD_DEFINITIONS(-DGL)
	ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)


# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DDEBUG)
	ADD_DEFINITIONS(-Wall)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-DRELEASE)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Executable definition
ADD_EXECUTABLE(CommanderGenius ${ALL_SRCS})

# Linking part under Linux
IF(BUILD_TARGET STREQUAL WIN32)
	TARGET_LINK_LIBRARIES(CommanderGenius mingw32)
	TARGET_LINK_LIBRARIES(CommanderGenius SDLmain)
	TARGET_LINK_LIBRARIES(CommanderGenius SDL)
	TARGET_LINK_LIBRARIES(CommanderGenius glu32)
	TARGET_LINK_LIBRARIES(CommanderGenius opengl32)
ENDIF(BUILD_TARGET STREQUAL WIN32)


IF(OPENGL)
    IF(BUILD_TARGET STREQUAL WIN32)
	TARGET_LINK_LIBRARIES(CommanderGenius opengl32)
    ELSE()
	TARGET_LINK_LIBRARIES(CommanderGenius GL)
    ENDIF()
ENDIF(OPENGL)

IF(OGG)
	TARGET_LINK_LIBRARIES(CommanderGenius vorbis vorbisfile)
ENDIF(OGG)

IF(TREMOR)
	TARGET_LINK_LIBRARIES(CommanderGenius vorbisidec)
ENDIF(TREMOR)

# Set binary(executable) file name. In Windows this filename needs the exe extension
# The Windows Version has an icon in the executable


IF(BUILD_TARGET STREQUAL WIN32)
	SET_TARGET_PROPERTIES(CommanderGenius PROPERTIES OUTPUT_NAME "CGenius.exe")
	ADD_CUSTOM_COMMAND(TARGET CommanderGenius PRE_LINK COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc )
	ADD_CUSTOM_COMMAND(TARGET CommanderGenius PRE_LINK COMMAND icotool -c -o ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.ico ${CMAKE_CURRENT_SOURCE_DIR}/vfsroot/cglogo512.png )
	ADD_CUSTOM_COMMAND(TARGET CommanderGenius PRE_LINK COMMAND /usr/bin/i686-w64-mingw32-windres ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o)
	SET_TARGET_PROPERTIES(CommanderGenius PROPERTIES LINK_FLAGS "-L/opt/mingw32/lib ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o")	
	LINK_DIRECTORIES(/opt/mingw32/lib)
	MESSAGE( "Will build with the CG-Icon for Windows." )
ENDIF(BUILD_TARGET STREQUAL WIN32)

target_link_libraries(CommanderGenius ${SDL_LIBRARIES})
#target_link_libraries(CommanderGenius ${SDL_IMAGE_LIBRARIES})
target_link_libraries(CommanderGenius SDL_image)


#MESSAGE( "BUILD_TARGET = ${BUILD_TARGET}" )
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
MESSAGE( "SDL_LIBRARIES = ${SDL_LIBRARIES}" )
MESSAGE( "SDL_IMAGE_LIBRARIES = ${SDL_IMAGE_LIBRARIES}" )

#MESSAGE( "Compile it with OpenGL : OPENGL = ${OPENGL}" )
#MESSAGE( "We use OGG Vorbis : OGG = ${OGG}" )
#MESSAGE( "We use Tremor : TREMOR = ${TREMOR}" )

#MESSAGE( "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}" )
#MESSAGE( "CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}" )
#MESSAGE( "You are on 64-bit System : HAVE_64_BIT = ${HAVE_64_BIT}" )
#MESSAGE( "USRDIR = ${USRDIR}" )
#MESSAGE( "Commander Genius Version : CG_VERSION = ${CG_VERSION}" )
#MESSAGE("VORBIS_INCLUDE_PATH = ${VORBIS_INCLUDE_PATH}")
