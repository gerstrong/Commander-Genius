# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.5)

Project(GsKit)
    
OPTION(USE_SDL2 "SDL2 support" Yes)

OPTION(USE_BOOST "Use Boost for XML" Yes)

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

option(USE_CRASHHANDLER "Enables the Crash Handler" ON)
if(USE_CRASHHANDLER)
    ADD_DEFINITIONS(-DUSE_CRASHHANDLER)
endif()



if(SDL2)
    OPTION(USE_OPENGL "OpenGL support" No)
else()
    OPTION(USE_OPENGL "OpenGL support" Yes)
endif()


if(USE_BOOST)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREAD ON)
    set(Boost_USE_STATIC OFF)

    FIND_PACKAGE(Boost)
    IF (Boost_FOUND)
            INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
            ADD_DEFINITIONS( "-DBOOST_ENABLED" )
    ELSE(Boost_FOUND)
            MESSAGE(FATAL_ERROR "Could not find the Boost libraries")
    ENDIF(Boost_FOUND)
endif(USE_BOOST)


IF (Boost_FOUND)
    # Remove tinyxml2 if boost will be used
    list(REMOVE_ITEM ALL_SRCS_GSKIT_BASE
            ${CMAKE_CURRENT_SOURCE_DIR}/base/utils/property_tree/tinyxml2.h
            ${CMAKE_CURRENT_SOURCE_DIR}/base/utils/property_tree/tinyxml2.cpp)
endif(Boost_FOUND)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")

  set(CMAKE_CXX_FLAGS_INIT "-s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "-s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT}")
  set(CMAKE_EXE_LINKER "${CMAKE_EXE_LINKER_FLAGS_INIT} -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]'")

  # TODO: Needs rework
  add_library(GsKit STATIC ${ALL_SRCS_GSKIT_BASE})        

else()
    
    if(SDL2)
        OPTION(USE_OPENGL "OpenGL support" No)
    else()
        OPTION(USE_OPENGL "OpenGL support" Yes)
    endif()
    
    option(USE_VIRTUALPAD "Enable Onscreen Virtual Gamepad support " ON)
    
    if(USE_VIRTUALPAD)
        ADD_DEFINITIONS(-DVIRTUALPAD)
    endif(USE_VIRTUALPAD)
    
    set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake")

    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    IF(WIN32)
    	ADD_DEFINITIONS(-DWIN32)
    ENDIF(WIN32)
    
    IF (USE_OPENGL)
            ADD_DEFINITIONS(-DGL)
            ADD_DEFINITIONS(-DUSE_OPENGL)
    ENDIF (USE_OPENGL)
    
    
    # If we want to debug set the proper flags or have release etc.
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
            ADD_DEFINITIONS(-DDEBUG)
            ADD_DEFINITIONS(-g)
            ADD_DEFINITIONS(-O0)
            ADD_DEFINITIONS(-Wall)
    ELSEIF(CMAKE_BUILD_TYPE STREQUAL "Release")
            ADD_DEFINITIONS(-DRELEASE)
            ADD_DEFINITIONS(-O2)
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    
    
    if(USE_SDL2)
    
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
    
        if(SDL2_FOUND)
            message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")
            include_directories(${SDL2_INCLUDE_DIRS})
    
            message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")
    
            include_directories(${SDL2_INCLUDE_DIR})
            include_directories(${SDL2_NET_INCLUDE_DIRS})
            include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
            include_directories(${SDL2_TTF_INCLUDE_DIRS})
            #include_directories(${SDL2_NET_INCLUDE_DIRS})
    
        endif(SDL2_FOUND)
    
    else(USE_SDL2)
            INCLUDE(FindPkgConfig)
            # SDL2 not found, try to use SDL1.2
            FIND_PACKAGE( SDL REQUIRED )
            FIND_PACKAGE( SDL_image REQUIRED )
            find_package( SDL_ttf REQUIRED )
    
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
    
    
    IF(USE_OPENGL)
        # OpenGL Parsing
        set(OpenGL_GL_PREFERENCE GLVND)
        find_package(OpenGL REQUIRED)    
    
        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
    ENDIF (USE_OPENGL)
    

    add_library(${PROJECT_NAME} LICENSE)

    add_subdirectory(base)
    add_subdirectory(fileio)
    add_subdirectory(graphics)
    add_subdirectory(widgets)

    Target_link_libraries(${PROJECT_NAME} GsKit_base)
    Target_link_libraries(${PROJECT_NAME} GsKit_fileio)
    Target_link_libraries(${PROJECT_NAME} GsKit_widgets)
    
    # The naming with the new Find cmake files related
    # to SDL is not always consistent. Some use the "2" as var name others not.
    if(USE_SDL2)
        Target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
        Target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
        Target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARIES})
        Target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARY})
        Target_link_libraries(${PROJECT_NAME} ${SDLIMAGE_LIBRARY})
        Target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})
    else()
        Target_link_libraries(${PROJECT_NAME} ${SDL_LIBRARY})
        Target_link_libraries(${PROJECT_NAME} ${SDL_LIBRARIES})
        Target_link_libraries(${PROJECT_NAME} ${SDL_IMAGE_LIBRARY})
        Target_link_libraries(${PROJECT_NAME} ${SDL_TTF_LIBRARY})
    endif()
    
if(USE_SDL2)
        message(STATUS "${PROJECT_NAME}: SDL2_LIBRARY = ${SDL2_LIBRARY}")
        message(STATUS "${PROJECT_NAME}: SDL2IMAGE_LIBRARY = ${SDL2IMAGE_LIBRARY}")
endif(USE_SDL2)
    
IF(USE_OPENGL)
    # OpenGL stuff to link
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
ENDIF(USE_OPENGL)

endif()

