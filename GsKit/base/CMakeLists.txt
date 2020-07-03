# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.5)

Project(GsKit_base)

add_subdirectory(lua)
add_subdirectory(utils)
add_subdirectory(video)

set(HEADERS CInput.h        GsEventContainer.h  GsPython.h        PointDevice.h
Configurator.h  GsEvent.h           GsTimer.h         Singleton.h
GsApp.h         GsGear.h            GsTTFSystem.h     TypeDefinitions.h
GsArguments.h   GsLogging.h         GsVirtualinput.h  Vector2D.h
GsEngine.h      GsLua.h             InputEvents.h)

set(SRCS CInput.cpp       GsEngine.cpp          GsLogging.cpp  GsTimer.cpp
GsApp.cpp        GsEventContainer.cpp  GsLua.cpp      GsTTFSystem.cpp
GsArguments.cpp  GsEvent.cpp           GsPython.cpp   TypeDefinitions.cpp
)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_base_lua)
target_link_libraries(${PROJECT_NAME} GsKit_base_utils)
target_link_libraries(${PROJECT_NAME} GsKit_base_video)
