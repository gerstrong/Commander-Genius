
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB SRCS *.cpp *.h)

add_library(fileio OBJECT ${SRCS})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:fileio>)
