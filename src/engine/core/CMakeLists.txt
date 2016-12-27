
include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB_RECURSE srcs *.cpp *.h)

add_library(engine_core OBJECT ${srcs})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine_core>)
