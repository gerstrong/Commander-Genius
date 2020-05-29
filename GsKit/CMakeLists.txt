# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.5)

Project(GsKit)
    
OPTION(USE_SDL2 "SDL2 support" Yes)


if(SDL2)
    OPTION(USE_OPENGL "OpenGL support" No)
else()
    OPTION(USE_OPENGL "OpenGL support" Yes)
endif()


# From this point files are globbed for compilation and project setup
file(GLOB_RECURSE ALL_SRCS_GSKIT_BASE *.c* *.h*)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


if(CMAKE_SYSTEM_NAME MATCHES "Emscripten")

  set(CMAKE_CXX_FLAGS_INIT "-s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'")
  set(CMAKE_EXE_LINKER_FLAGS_INIT "-s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]'")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT}")
  set(CMAKE_EXE_LINKER "${CMAKE_EXE_LINKER_FLAGS_INIT} -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]'")


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
    
    option(USE_PYTHON3 "Use Python3 scripts" no)
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
            ADD_DEFINITIONS(-O3)
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    
    
    if(USE_SDL2)
    
        FIND_PACKAGE(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
    
        if(SDL2_FOUND)
            message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")
            include_directories(${SDL2_INCLUDE_DIRS})
    
            message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")
    
            if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
                set(SDL2_INCLUDE_DIRS /Library/Frameworks/SDL2.framework/Headers)
                set(SDL2_IMAGE_INCLUDE_DIRS /Library/Frameworks/SDL2_image.framework/Headers)
            endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    
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
    
    
    # Use python3 for AI scripting and more!
    IF(USE_PYTHON3)
    
        if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
            FIND_PACKAGE(PythonLibs REQUIRED)
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
    endif()
    
    
    IF(USE_OPENGL)
        # OpenGL Parsing
        set(OpenGL_GL_PREFERENCE GLVND)
        find_package(OpenGL REQUIRED)    
    
        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
    ENDIF (USE_OPENGL)
    
    add_library(GsKit STATIC ${ALL_SRCS_GSKIT_BASE})
    
    # The naming with the new Find cmake files related
    # to SDL is not always consistent. Some use the "2" as var name others not.
    if(USE_SDL2)
        Target_link_libraries(GsKit ${SDL2_LIBRARIES})
        Target_link_libraries(GsKit ${SDL2_LIBRARY})
        Target_link_libraries(GsKit ${SDL2IMAGE_LIBRARIES})
        Target_link_libraries(GsKit ${SDLIMAGE_LIBRARY})
        Target_link_libraries(GsKit ${SDL2_TTF_LIBRARY})
    else()
        Target_link_libraries(GsKit ${SDL_LIBRARY})
        Target_link_libraries(GsKit ${SDL_LIBRARIES})
        Target_link_libraries(GsKit ${SDL_IMAGE_LIBRARY})
        Target_link_libraries(GsKit ${SDL_TTF_LIBRARY})
    endif()
    
if(USE_SDL2)
        message(STATUS "GsKit: SDL2_LIBRARY = ${SDL2_LIBRARY}")
        message(STATUS "GsKit: SDL2IMAGE_LIBRARY = ${SDL2IMAGE_LIBRARY}")
endif(USE_SDL2)
    
    IF(USE_OPENGL)
        # OpenGL stuff to link
        target_link_libraries(GsKit ${OPENGL_LIBRARIES})
    ENDIF(USE_OPENGL)
    
    #cotire(GsKit)
endif()
