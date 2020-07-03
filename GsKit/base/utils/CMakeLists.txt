# CMake file for development of Commander Genius (taken from OLX)
# This CMake file is used under Linux normally. 
    
cmake_minimum_required(VERSION 3.5)

Project(GsKit_base_utils)

add_subdirectory(property_tree)


set(HEADERS Base64.h         Event.h       MathLib.h        Ref.h          Utils.h
Color.h          EventQueue.h  misc.h           StringBuf.h
ConfigHandler.h  FindFile.h    Mutex.h          StringUtils.h
CrashHandler.h   Geometry.h    ReadWriteLock.h  ThreadPool.h
Debug.h          Iterator.h    RefCounter.h     Unicode.h
)

set(SRCS Base64.cpp         CrashHandler.cpp  Geometry.cpp     ThreadPool.cpp
Color.cpp          Debug.cpp         StringBuf.cpp    Unicode.cpp
ConfigHandler.cpp  FindFile.cpp      StringUtils.cpp
)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})

target_link_libraries(${PROJECT_NAME} GsKit_base_utils_ptree)
