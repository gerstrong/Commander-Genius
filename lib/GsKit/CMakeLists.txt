# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

#cmake_minimum_required(VERSION 3.1)
cmake_minimum_required(VERSION 2.8.12)

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif ()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

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

    IF(WIN32)
      include(FindSDL2 OPTIONAL)
    else()
      INCLUDE(FindPkgConfig)
      find_package(SDL2 REQUIRED)
      find_package(SDL2_image REQUIRED >=2.0.0)
      #PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
      #PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

	
    endif()

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for Commander Genius")
        include_directories(${SDL2_INCLUDE_DIRS})



        set(SDL2_INCLUDE_DIRS /Library/Frameworks/SDL2.framework/Headers)
        set(SDL2_IMAGE_INCLUDE_DIRS /Library/Frameworks/SDL2_image.framework/Headers)   

        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
        #include_directories(${SDL2_NET_INCLUDE_DIRS})

        #include_directories(/usr/include/SDL2/)
    endif(SDL2_FOUND)

else(USE_SDL2)
        INCLUDE(FindPkgConfig)
        # SDL2 not found, try to use SDL1.2
        FIND_PACKAGE( SDL REQUIRED )
        FIND_PACKAGE( SDL_image REQUIRED )
        #PKG_SEARCH_MODULE(SDL_IMAGE REQUIRED SDL_image)

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
    # Look for Python (Version 3.0 or later is required)
    FIND_PACKAGE(PythonLibs)
    ADD_DEFINITIONS(-DUSE_PYTHON3=1)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
    LINK_LIBRARIES(${PYTHON_LIBRARIES})
ENDIF()


Project(GsKit)

# From this point files are globbed for compilation and project setup
file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})



