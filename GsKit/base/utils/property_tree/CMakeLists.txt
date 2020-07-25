cmake_minimum_required(VERSION 3.5)

Project(GsKit_base_utils_ptree)

set(HEADERS property_tree.h  tinyxml2.h  xml_parser.h)

set(SRCS property_tree.cpp  tinyxml2.cpp  xml_parser.cpp)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
