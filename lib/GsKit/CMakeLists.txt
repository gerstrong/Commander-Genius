# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 

cmake_minimum_required(VERSION 2.8)

# Make this C++11. We need that in GsKit
ADD_DEFINITIONS(-std=c++11)

Project(GsKit)

if(SDL2)

    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

    include(FindSDL2 OPTIONAL)

    if(SDL2_FOUND)
        message("Using shared SDL Version 2")
        include_directories(${SDL2_INCLUDE_DIR})
        include_directories(/usr/include/SDL2/)
    endif(SDL2_FOUND)

else(SDL2)
        # SDL2 not found, try SDL
        FIND_PACKAGE( SDL REQUIRED )
        if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                include(FindSDL OPTIONAL)
        endif()
        if(SDL_FOUND)
                message("Using shared SDL Version 1.2")
                include_directories(${SDL_INCLUDE_DIR})
                INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIRS})
        else(SDL_FOUND)
                # TODO: Use the prebuilt one on Windows
                message("Using static SDL from Externals")
                include_directories(Externals/SDL Externals/SDL/include)
                add_subdirectory(Externals/SDL)
        endif(SDL_FOUND)
endif(SDL2)


#IF(BUILD_TARGET STREQUAL WIN32)
#    set(SDL_INCLUDE_DIRS ${PREFIX}/include/SDL)
#    include_directories(${SDL_INCLUDE_DIRS})
#ELSE(BUILD_TARGET STREQUAL WIN32)
#    find_package(PkgConfig)
#    pkg_check_modules(SDL sdl)
#ENDIF(BUILD_TARGET STREQUAL WIN32)

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




# TODO: From this point we should recursively glob the files. I think it is the easiest way but maybe not the best, I don't know yet.

file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library(GsKit ${ALL_SRCS_GSKIT_BASE})


if(SDL2_FOUND)
    target_link_libraries(GsKit ${SDL2_LIBRARIES})
    # Workaround since there is no SDL2_Image cmake yet
    set(SDL_IMAGE_LIBRARIES "SDL2_image")
    target_link_libraries(GsKit ${SDL_IMAGE_LIBRARIES})
else(SDL2_FOUND)
    target_link_libraries(GsKit ${SDL_LIBRARY})
    target_link_libraries(GsKit ${SDLIMAGE_LIBRARY})
endif(SDL2_FOUND)



