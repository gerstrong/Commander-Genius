cmake_minimum_required(VERSION 3.8)

Project(GsKit_fileio)

set(HEADERS CConfiguration.h  IniReader.h  ResourceMgmt.h fileio.h)

set(SRCS CConfiguration.cpp  IniReader.cpp  ResourceMgmt.cpp fileio.cpp)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
