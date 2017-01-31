# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables

OPTION(TOUCHCONTROLS "Touchcontrols support" No)

if(SDL2)
    OPTION(OPENGL "OpenGL support" No)
else()
    OPTION(OPENGL "OpenGL support" Yes)
endif()


IF(NOT DEFINED BUILD_TARGET)
IF(WIN32)
	SET(BUILD_TARGET WIN32)
		ADD_DEFINITIONS(-DWIN32)
		ADD_DEFINITIONS(-DTARGET_WIN32)
ELSE(WIN32)
	SET(BUILD_TARGET LINUX)
ENDIF(WIN32)
MESSAGE( STATUS "WARNING: You did not specify the build type with the -DBUILD_TARGET= Parameter. ${BUILD_TARGET} has been choosen" )
MESSAGE( STATUS "Available builds are: WIN32, LINUX GP2X. WIZ, CAANOO, DINGOO, NANONOTE" )
ENDIF(NOT DEFINED BUILD_TARGET)


SET(USRDIR /usr)

include(oldArch.cmake)


if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

#Alignment error report is important for android builds
ADD_DEFINITIONS(-Werror=cast-align)



# Directory creation of all the chosen build
#set(BUILD_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build")
#set(BUILD_DIR "${BUILD_BASE_DIR}/${BUILD_TARGET}")
#execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_BASE_DIR})
#execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_DIR})

#SET(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR})

# Use python3 for AI scripting and more!
IF(USE_PYTHON3)
    FIND_PACKAGE(PythonLibs)
    ADD_DEFINITIONS(-DUSE_PYTHON3=1)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    LINK_LIBRARIES(${PYTHON_LIBRARIES})
ENDIF()


# main includes
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/GsKit)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/)


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
	FIND_PATH(TREMOR_INCLUDE_DIR
		ivorbisfile.h
		PATHS /usr/include /usr/include/tremor
		DOC "Include for Tremor")
	INCLUDE_DIRECTORIES(${TREMOR_INCLUDE_DIR})
	ADD_DEFINITIONS(-DTREMOR)
ENDIF (TREMOR)



# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DDEBUG)
        ADD_DEFINITIONS(-g)
        ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-Wall)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-DRELEASE)
        ADD_DEFINITIONS(-O3)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")



# Make sure the compiler can find include files from our Gskit library.
#include_directories (${CommanderGenius_SOURCE_DIR}/../lib/GsKit)

# Make sure the linker can find the GsKit library once it is built.
link_directories (${CommanderGenius_BINARY_DIR}/lib/GsKit)

# We need that so we can compile dosbox fusion
#include_directories (${CommanderGenius_SOURCE_DIR}/src/engine/dbfusion)
#include_directories (${CommanderGenius_SOURCE_DIR}/src/engine/dbfusion/include)


set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
set(Boost_USE_STATIC OFF)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	ADD_DEFINITIONS( "-DHAS_BOOST" )
ELSE(Boost_FOUND)
	MESSAGE(FATAL_ERROR "Could not find the Boost libraries")
ENDIF(Boost_FOUND)


find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
endif( ZLIB_FOUND )

if(DOWNLOADER)
    FIND_PACKAGE(CURL)
    IF(CURL_FOUND)
      INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
    ELSE(CURL_FOUND)
      MESSAGE(FATAL_ERROR "Could not find the CURL library and development files." )
    ENDIF(CURL_FOUND)
endif()

if(USE_SDL2)

      find_package(SDL2 REQUIRED)
      find_package(SDL2_image REQUIRED)
#    IF(WIN32)
#      include(FindSDL2 OPTIONAL)      
#    else()
#      INCLUDE(FindPkgConfig)
      #PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
      #PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
#    endif()

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIRS})
        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

        #include_directories(${SDL2_NET_INCLUDE_DIRS})

        include_directories(/usr/include/SDL2/)
    endif(SDL2_FOUND)

else(USE_SDL2)
        INCLUDE(FindPkgConfig)
        # SDL2 not found, try to use SDL1.2
        FIND_PACKAGE( SDL REQUIRED )
        FIND_PACKAGE( SDL_image REQUIRED )
        #PKG_SEARCH_MODULE(SDLIMAGE REQUIRED SDL_image)

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

IF (OPENGL)
        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)

IF (TOUCHCONTROLS)
        ADD_DEFINITIONS(-DTOUCHCONTROLS)
ENDIF (TOUCHCONTROLS)



MESSAGE( STATUS "DOSBox-Fusion: ${DBFUSION}" )
MESSAGE( STATUS "Internal Downloader: ${DOWNLOADER}" )

if(DBFUSION)
	if(WIN32)
		# For the Windows build you need this flag or is dosbox part is going to hang while mounting the drive
		ADD_DEFINITIONS("-fpermissive -mno-ms-bitfields")
	endif(WIN32)
endif(DBFUSION)


add_subdirectory(engine)
add_subdirectory(sdl)
add_subdirectory(fileio)
add_subdirectory(dialog)
add_subdirectory(graphics)

# Get all the o-files from the different targets and link'em
get_property(cg_obj_libs GLOBAL PROPERTY CG_OBJ_LIBS)

# Add executable Commander Genius
add_executable (CGeniusExe CGenius.cpp fileio.cpp misc.cpp ${cg_obj_libs})

# Since some RefKeen is used, we also need C99 Standard
set_property(TARGET CGeniusExe PROPERTY C_STANDARD 99)


# Link GsKit
target_link_libraries (CGeniusExe GsKit)


# Link zlib
target_link_libraries( CGeniusExe ${ZLIB_LIBRARIES} )


if(USE_SDL2)
        target_link_libraries(CGeniusExe ${SDL2_LIBRARY})
        target_link_libraries(CGeniusExe ${SDLIMAGE_LIBRARY})
	message(STATUS "SDL2_LIBRARY = ${SDL2_LIBRARY}")
	message(STATUS "SDLIMAGE_LIBRARY = ${SDLIMAGE_LIBRARY}")
else(USE_SDL2)
        # SDL stuff to link
        target_link_libraries(CGeniusExe ${SDL_NET_LIBRARIES})
        target_link_libraries(CGeniusExe ${SDL_LIBRARY})
        target_link_libraries(CGeniusExe ${SDL_IMAGE_LIBRARY})
endif(USE_SDL2)


IF(OPENGL)
    # OpenGL Parsing
    find_package(OpenGL REQUIRED)

    IF(WIN32)
	TARGET_LINK_LIBRARIES(CGeniusExe opengl32)
    ELSE()
	TARGET_LINK_LIBRARIES(CGeniusExe GL)
    ENDIF()
ENDIF(OPENGL)

# TODO: Does not work with MacOS. Where is the pkgconfig or cmake script
IF(OGG)
	if(WIN32)
        TARGET_LINK_LIBRARIES(CGeniusExe vorbisfile vorbis ogg)
	else(WIN32)
		TARGET_LINK_LIBRARIES(CGeniusExe vorbis vorbisfile)
	endif(WIN32)
ENDIF(OGG)

IF(TREMOR)
	TARGET_LINK_LIBRARIES(CGeniusExe vorbisidec)
ENDIF(TREMOR)

IF(CURL_FOUND)
  TARGET_LINK_LIBRARIES(CGeniusExe ${CURL_LIBRARIES})
ENDIF(CURL_FOUND)

# Set binary(executable) file name. In Windows this filename needs the exe extension
# The Windows Version has an icon in the executable


IF(WIN32)
	SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES OUTPUT_NAME "CGenius")
#	ADD_CUSTOM_COMMAND(TARGET CGeniusExe PRE_LINK COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc )
#	ADD_CUSTOM_COMMAND(TARGET CGeniusExe PRE_LINK COMMAND ${mingw64dir}bin/icotool -c -o ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.ico ${CMAKE_CURRENT_SOURCE_DIR}/vfsroot/cglogo512.png )
	ADD_CUSTOM_COMMAND(TARGET CGeniusExe PRE_LINK COMMAND windres ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o)
	SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES LINK_FLAGS "-mwindows ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o")
#	LINK_DIRECTORIES(/opt/mingw32/lib)
	MESSAGE(STATUS "Will build with the CG-Icon for Windows." )
ENDIF(WIN32)



# Only for SDL2 and Win32 since it is updated for that platform. SDL 1.2 must be tested in case someone wants to use it on windows
if(WIN32)
	target_link_libraries(CGeniusExe mingw32)
	target_link_libraries(CGeniusExe user32)
	target_link_libraries(CGeniusExe gdi32)
	target_link_libraries(CGeniusExe SDL2Main SDL2)
	target_link_libraries(CGeniusExe SDL2_image)
	target_link_libraries(CGeniusExe winmm)
	target_link_libraries(CGeniusExe Imm32)
	target_link_libraries(CGeniusExe Version)
endif(WIN32)

target_link_libraries(CGeniusExe ${PYTHON_LIBRARIES})

IF(OPENGL)
    # OpenGL stuff to link
    target_link_libraries(CGeniusExe ${OPENGL_LIBRARIES})
ENDIF(OPENGL)

# Only if you want to use Dosfusion
if(DBFUSION)
    #target_link_libraries(CGeniusExe SDL_sound)

    target_link_libraries(CGeniusExe m)
    target_link_libraries(CGeniusExe png)

	if(WIN32)
		target_link_libraries(CGeniusExe SDL2_net)
	else()
		target_link_libraries(CGeniusExe asound)
		target_link_libraries(CGeniusExe X11)
		target_link_libraries(CGeniusExe pthread)
		target_link_libraries(CGeniusExe dl)
	endif(WIN32)

endif(DBFUSION)

LINK_DIRECTORIES(/opt/local/lib)

cotire(CGeniusExe)

# Stuff definitions in case we want to install it
INCLUDE(install.cmake)
