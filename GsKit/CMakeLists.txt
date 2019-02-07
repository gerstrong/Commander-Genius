# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 3.7)

Project(GsKit)

OPTION(USE_SDL2 "SDL2 support" Yes)
option(USE_PYTHON3 "Use Python3 scripts" yes)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(WIN32)
	ADD_DEFINITIONS(-DWIN32)
ENDIF(WIN32)

IF (OPENGL)
        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)


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


if(USE_SDL2)

    FIND_PACKAGE(SDL2)

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIRS})

        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        set(SDL2_INCLUDE_DIRS /Library/Frameworks/SDL2.framework/Headers)
        set(SDL2_IMAGE_INCLUDE_DIRS /Library/Frameworks/SDL2_image.framework/Headers)

        include_directories(${SDL2_INCLUDE_DIR})
        include_directories(${SDL2_NET_INCLUDE_DIRS})

        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
        #include_directories(${SDL2_NET_INCLUDE_DIRS})        

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
                INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})
        else(SDL_FOUND)
                # TODO: Use the prebuilt one on Windows
                message("Using static SDL from Externals")
                include_directories(Externals/SDL Externals/SDL/include)
                add_subdirectory(Externals/SDL)
        endif(SDL_FOUND)

endif(USE_SDL2)


# Use python3 for AI scripting and more!
IF(USE_PYTHON3)

    if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
        FIND_PACKAGE(PythonLibs REQUIRED)
        TARGET_LINK_LIBRARIES(GsKit ${PYTHON_LIBRARIES})
    else()
        # Look for Python (Version 3.0 or later is required)
        FIND_PACKAGE(Python3 COMPONENTS Interpreter Development)
        if(Python3_FOUND)
            ADD_DEFINITIONS(-DUSE_PYTHON3=1)
            INCLUDE_DIRECTORIES(${Python3_INCLUDE_DIRS})
            LINK_LIBRARIES(${Python3_LIBRARIES})
            message(STATUS "Python3_LIBRARIES = ${Python3_LIBRARIES}")
            message(STATUS "Python3_INCLUDE_DIRS = ${Python3_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "Python3 not found!")
        endif()
    endif()

ENDIF()





# From this point files are globbed for compilation and project setup
file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


if(USE_SDL2)
    find_package(SDL2 REQUIRED)

    if(SDL2_FOUND)
        find_package(SDL2_image REQUIRED)

        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIRS})
        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

    endif(SDL2_FOUND)
endif()


IF(OPENGL)
    # OpenGL Parsing
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED)    

    ADD_DEFINITIONS(-DGL)
    ADD_DEFINITIONS(-DUSE_OPENGL)
ENDIF (OPENGL)

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})

# NOTE: The naming with the new Find cmake files is strange sometimes.
#       Some use the "2" as var name others not.
if(USE_SDL2)
    Target_link_libraries(GsKit ${SDL2_LIBRARIES})
    Target_link_libraries(GsKit ${SDL2_LIBRARY})
    Target_link_libraries(GsKit ${SDL2IMAGE_LIBRARIES})
    Target_link_libraries(GsKit ${SDLIMAGE_LIBRARY})
else()
    Target_link_libraries(GsKit ${SDL_LIBRARY})
    Target_link_libraries(GsKit ${SDL_LIBRARIES})
    Target_link_libraries(GsKit ${SDL_IMAGE_LIBRARY})
endif()


IF(OPENGL)
    # OpenGL stuff to link
    target_link_libraries(GsKit ${OPENGL_LIBRARIES})
ENDIF(OPENGL)

cotire(GsKit)
