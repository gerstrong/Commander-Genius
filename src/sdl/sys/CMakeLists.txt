
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_sys OBJECT wizgp2x.cpp wizgp2x.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl_sys>)
