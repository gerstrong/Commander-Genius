Project(GsKit_base)


if(${CMAKE_VERSION} VERSION_GREATER "3.8.0")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
else()
    if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
      set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")
      set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++17")
    elseif (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
      set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++1z")
      set(CMAKE_CXX17_EXTENSION_COMPILE_OPTION "-std=gnu++1z")
    endif()
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(interface)
add_subdirectory(lua)
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

target_link_libraries(${PROJECT_NAME} GsKit_base_interface)
target_link_libraries(${PROJECT_NAME} GsKit_base_lua)
target_link_libraries(${PROJECT_NAME} GsKit_base_video)
target_link_libraries(${PROJECT_NAME} GsKit_base_audio)

target_link_libraries(${PROJECT_NAME} GsKit_widgets)

