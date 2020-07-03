cmake_minimum_required(VERSION 3.5)

Project(GsKit_fileio)

set(HEADERS CConfiguration.h  IniReader.h  ResourceMgmt.h)

set(SRCS CConfiguration.cpp  IniReader.cpp  ResourceMgmt.cpp)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
