cmake_minimum_required(VERSION 3.7)

Project(GsKit_base_utils_ptree)

set(HEADERS property_tree.h
            xml_parser.h)

set(SRCS property_tree.cpp
         xml_parser.cpp)

IF (NOT Boost_FOUND OR USE_BOOST)
        set(HEADERS ${HEADERS} tinyxml2.h)
        set(SRCS ${SRCS} tinyxml2.cpp)
endif(NOT Boost_FOUND OR USE_BOOST)

add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
