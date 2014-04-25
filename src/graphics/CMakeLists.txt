
include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB SRCS effects/*.cpp effects/*.h)

add_library(graphics OBJECT ${SRCS})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:graphics>)

