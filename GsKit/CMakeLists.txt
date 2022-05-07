# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally.

cmake_minimum_required(VERSION 3.7)

Project(GsKit)


OPTION(USE_BOOST "Use Boost for XML" No)

option(DISABLE_HOVER "Disables the hover over effect in the menus, useful for touch based devices" OFF)

if(DISABLE_HOVER)
    ADD_DEFINITIONS(-DDISABLE_HOVER)
endif()

option(USE_CRASHHANDLER "Enables the Crash Handler" ON)
if(USE_CRASHHANDLER)
    ADD_DEFINITIONS(-DUSE_CRASHHANDLER)
endif()

OPTION(USE_OPENGL "OpenGL support" No)


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

    if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
        set(CMAKE_CXX_STANDARD 17)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
    else()
        if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
          set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
          set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")
        elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
          set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++1z")
          set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++1z")
        endif()
    endif()

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

    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    if(SDL2_FOUND)
        message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")
        include_directories(${SDL2_INCLUDE_DIRS})

        message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")

        include_directories(${SDL2_INCLUDE_DIR})
        include_directories(${SDL2_NET_INCLUDE_DIRS})
        include_directories(${SDL2_MIXER_INCLUDE_DIRS})
        include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
        include_directories(${SDL2_TTF_INCLUDE_DIRS})

    endif(SDL2_FOUND)


    IF(USE_OPENGL)
        # OpenGL Parsing
        set(OpenGL_GL_PREFERENCE GLVND)
        find_package(OpenGL REQUIRED)

        ADD_DEFINITIONS(-DGL)
        ADD_DEFINITIONS(-DUSE_OPENGL)
    ENDIF (USE_OPENGL)


    # Hack which on some systems helps linking this lib correctly
    # (required for mac builds)
    file(WRITE null.cpp "")
    add_library(${PROJECT_NAME} LICENSE null.cpp)

    add_subdirectory(fileio)
    add_subdirectory(graphics)
    add_subdirectory(widgets)

    add_subdirectory(base)

    Target_link_libraries(${PROJECT_NAME} GsKit_base)
    Target_link_libraries(${PROJECT_NAME} GsKit_fileio)
    Target_link_libraries(${PROJECT_NAME} GsKit_widgets)

    # The naming with the new Find cmake files related
    # to SDL is not always consistent. Some use the "2" as var name others not.
    Target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    Target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
    Target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARIES})
    Target_link_libraries(${PROJECT_NAME} ${SDL2IMAGE_LIBRARY})
    Target_link_libraries(${PROJECT_NAME} ${SDL2MIXER_LIBRARIES})
    Target_link_libraries(${PROJECT_NAME} ${SDL2MIXER_LIBRARY})
    Target_link_libraries(${PROJECT_NAME} ${SDLIMAGE_LIBRARY})
    Target_link_libraries(${PROJECT_NAME} ${SDLMIXER_LIBRARY})
    Target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})

IF(USE_OPENGL)
    # OpenGL stuff to link
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
ENDIF(USE_OPENGL)


endif()

