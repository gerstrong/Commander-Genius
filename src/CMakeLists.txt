# Platform specific things can be put here
# Compilers and other specific variables can be found here:
# http://www.cmake.org/Wiki/CMake_Useful_Variables


if(SDL2)
    OPTION(OPENGL "OpenGL support" No)
else()
    OPTION(OPENGL "OpenGL support" Yes)
endif()

OPTION(DOWNLOADER "Internal Downloader" Yes)


IF(WIN32)
	SET(BUILD_TARGET WIN32)
        ADD_DEFINITIONS(-DWIN32)
        ADD_DEFINITIONS(-DTARGET_WIN32)        
ELSEIF(UNIX)
	SET(BUILD_TARGET LINUX)
ENDIF()

IF(APPLE)
    SET(BUILD_TARGET APPLE)
endif()

IF(NOT DEFINED BUILD_TARGET)
    MESSAGE( STATUS "WARNING: You did not specify the build type with the -DBUILD_TARGET= Parameter. ${BUILD_TARGET} has been choosen" )
    MESSAGE( STATUS "Available builds are: WIN32, LINUX GP2X. WIZ, CAANOO, DINGOO, NANONOTE" )
ENDIF(NOT DEFINED BUILD_TARGET)


SET(USRDIR /usr)

include(oldArch.cmake)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#Alignment error report is important for android builds
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=cast-align")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=cast-align")

# Use python3 for AI scripting and more!
# Use python3 for AI scripting and more!
IF(USE_PYTHON3)
    # Look for Python (Version 3.0 or later is required)
    FIND_PACKAGE(Python3 COMPONENTS Interpreter Development)
    if(Python3_FOUND)
        ADD_DEFINITIONS(-DUSE_PYTHON3=1)
        INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
        LINK_LIBRARIES(${Python3_LIBRARIES})
        message(STATUS "Python3_LIBRARIES = ${Python3_LIBRARIES}")
        message(STATUS "Python3_INCLUDE_DIRS = ${Python3_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR " Python3 not found!")
    endif()
ENDIF()


# main includes
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/GsKit)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)

# If we want to debug set the proper flags or have release etc.
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	ADD_DEFINITIONS(-DDEBUG)
        ADD_DEFINITIONS(-g)
        ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-Wall)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
	ADD_DEFINITIONS(-DRELEASE)
        ADD_DEFINITIONS(-O3)
ENDIF()


# Make sure the linker can find the GsKit library once it is built.
link_directories (${CommanderGenius_BINARY_DIR}/lib/GsKit)


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



if(USE_SDL2)

      find_package(SDL2 REQUIRED)
      find_package(SDL2_image REQUIRED)

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIR})
        include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    endif(SDL2_FOUND)

else(USE_SDL2)
        INCLUDE(FindPkgConfig)
        # SDL2 not found, try to use SDL1.2
        FIND_PACKAGE( SDL REQUIRED )
        FIND_PACKAGE( SDL_image REQUIRED )

        if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                include(FindSDL OPTIONAL)
        endif()
        if(SDL_FOUND)
                message(STATUS "Using shared SDL Version 1.2")
                include_directories(${SDL_INCLUDE_DIR})
                #INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})
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

if(USE_SDL2)
	find_package(SDL2_mixer REQUIRED)

	if(SDL2_MIXER_FOUND)
    		INCLUDE_DIRECTORIES(${SDL2_MIXER_INCLUDE_DIRS})
	endif()

else()
	find_package(SDL_mixer REQUIRED)

	if(SDL_MIXER_FOUND)
    		INCLUDE_DIRECTORIES(${SDL_MIXER_INCLUDE_DIRS})
	endif()

endif()





MESSAGE( STATUS "Internal Downloader: ${DOWNLOADER}" )

add_subdirectory(engine)
add_subdirectory(sdl)
add_subdirectory(fileio)
add_subdirectory(dialog)
add_subdirectory(graphics)

# Get all the o-files from the different targets and link'em
get_property(cg_obj_libs GLOBAL PROPERTY CG_OBJ_LIBS)

# Add executable Commander Genius
add_executable (CGeniusExe CGenius.cpp
                fileio.cpp misc.cpp
                CRingBuffer.h
                fileio.h
                ${CMAKE_CURRENT_SOURCE_DIR}/../changelog.txt
                ${CMAKE_CURRENT_SOURCE_DIR}/../README.in
                ${cg_obj_libs})

# Since some RefKeen is used, we also need C99 Standard
set_property(TARGET CGeniusExe PROPERTY C_STANDARD 99)

# Link CG Engines
target_link_libraries (CGeniusExe engine)

# Link GsKit
target_link_libraries (CGeniusExe GsKit)

# Link our SDL Extensions
target_link_libraries(CGeniusExe sdl_extensions_audio sdl_extensions_sys)


if(USE_SDL2)
        message(STATUS "SDL2_LIBRARY = ${SDL2_LIBRARY}")
        message(STATUS "SDL2IMAGE_LIBRARY = ${SDL2IMAGE_LIBRARY}")
endif(USE_SDL2)



# The Windows Version has an icon in the executable
IF(WIN32)
	SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES OUTPUT_NAME "CGenius")
	ADD_CUSTOM_COMMAND(TARGET CGeniusExe PRE_LINK COMMAND windres ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.rc ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o)
	SET_TARGET_PROPERTIES(CGeniusExe PROPERTIES LINK_FLAGS "-mwindows ${CMAKE_CURRENT_SOURCE_DIR}/CGLogo.o")
	MESSAGE(STATUS "Will build with the CG-Icon for Windows." )
ENDIF(WIN32)



# Only for SDL2 and Win32 since it is updated for that platform. SDL 1.2 must be tested in case someone wants to use it on windows
if(WIN32)
	target_link_libraries(CGeniusExe mingw32)
	target_link_libraries(CGeniusExe user32)
	target_link_libraries(CGeniusExe gdi32)
        target_link_libraries(CGeniusExe SDL2Main)
	target_link_libraries(CGeniusExe winmm)
	target_link_libraries(CGeniusExe Imm32)
	target_link_libraries(CGeniusExe Version)
endif(WIN32)

target_link_libraries(CGeniusExe ${PYTHON_LIBRARIES})


IF(OPENGL)
    # OpenGL stuff to link
    target_link_libraries(CGeniusExe ${OPENGL_LIBRARIES})
ENDIF(OPENGL)


cotire(CGeniusExe)

# Stuff definitions in case we want to install it
INCLUDE(install.cmake)
