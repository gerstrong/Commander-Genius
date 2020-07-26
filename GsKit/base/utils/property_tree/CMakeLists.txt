cmake_minimum_required(VERSION 3.5)

Project(GsKit_base_utils_ptree)

set(HEADERS property_tree.h xml_parser.h)

set(SRCS property_tree.cpp xml_parser.cpp)

IF (Boost_FOUND)
else()
	set(HEADERS "${HEADERS} tinyxml2.h")
	set(SRCS "${SRCS} tinyxml2.cpp")
endif(Boost_FOUND)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
