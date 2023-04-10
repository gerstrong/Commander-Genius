# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally.

cmake_minimum_required(VERSION 3.7)

Project(GsKit_base_interface)

set(HEADERS Base64.h         Event.h       MathLib.h        Ref.h          Utils.h
Color.h          EventQueue.h  misc.h           StringBuf.h
ConfigHandler.h  FindFile.h    Mutex.h          StringUtils.h
CrashHandler.h   Geometry.h    ReadWriteLock.h  ThreadPool.h
Debug.h          Iterator.h    RefCounter.h     Unicode.h )

set(SRCS Base64.cpp         CrashHandler.cpp  Geometry.cpp     ThreadPool.cpp
Color.cpp          Debug.cpp         StringBuf.cpp    Unicode.cpp
ConfigHandler.cpp  FindFile.cpp      StringUtils.cpp  MathLib.cpp)

set(HEADERS ${HEADERS}
    property_tree/property_tree.h
    property_tree/xml_parser.h)

set(SRCS ${SRCS}
    property_tree/property_tree.cpp
    property_tree/xml_parser.cpp)

IF (NOT Boost_FOUND OR USE_BOOST)
        set(HEADERS ${HEADERS} property_tree/tinyxml2.h)
        set(SRCS ${SRCS} property_tree/tinyxml2.cpp)
endif(NOT Boost_FOUND OR USE_BOOST)

add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

if(SDL2_FOUND)
    message(STATUS "Using shared SDL Version 2 for ${PROJECT_NAME}")

    #target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})

    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${SDL2_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:${SDL2_INCLUDE_DIRS}>
    )


    target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${SDL2_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${SDL2_INCLUDE_DIR}>
    )

    message(STATUS "SDL2_INCLUDE_DIRS =  ${SDL2_INCLUDE_DIRS}")
    message(STATUS "SDL2_INCLUDE_DIR =  ${SDL2_INCLUDE_DIR}")
    message(STATUS "SDL2_MIXER_INCLUDE_DIRS =  ${SDL2_MIXER_INCLUDE_DIRS}")
    message(STATUS "SDL2_IMAGE_INCLUDE_DIRS =  ${SDL2_IMAGE_INCLUDE_DIRS}")
    message(STATUS "SDL2_TTF_INCLUDE_DIRS =  ${SDL2_TTF_INCLUDE_DIRS}")

else()
    message(FATAL_ERROR "SDL2 was not detected!")
endif(SDL2_FOUND)


target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
    ${SDL2_TTF_LIBRARIES})


