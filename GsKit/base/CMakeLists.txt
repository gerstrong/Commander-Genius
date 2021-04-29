# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.8)

Project(GsKit_base)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)


add_subdirectory(lua)
add_subdirectory(utils)
add_subdirectory(video)
add_subdirectory(audio)

include_directories(lua)

set(HEADERS CInput.h        GsEventContainer.h       PointDevice.h
Configurator.h  GsEvent.h           GsTimer.h         Singleton.h
GsApp.h         GsGear.h            GsTTFSystem.h     TypeDefinitions.h
GsArguments.h   GsLogging.h         GsVirtualinput.h  Vector2D.h
GsEngine.h      GsLua.h             InputEvents.h     CRingBuffer.h
direction.h )

set(SRCS CInput.cpp       GsEngine.cpp          GsLogging.cpp  GsTimer.cpp
GsApp.cpp        GsEventContainer.cpp  GsLua.cpp      GsTTFSystem.cpp
GsArguments.cpp  GsEvent.cpp           TypeDefinitions.cpp
)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_base_lua)
target_link_libraries(${PROJECT_NAME} GsKit_base_utils)
target_link_libraries(${PROJECT_NAME} GsKit_base_video)
