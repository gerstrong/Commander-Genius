
include_directories(${SDL_INCLUDE_DIR})

file(GLOB_RECURSE SRCS *.cpp *.h)

add_library(dbfusion_root OBJECT ${SRCS})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:dbfusion_root>)
