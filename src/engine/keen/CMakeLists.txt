
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(galaxy)
add_subdirectory(vorticon)
add_subdirectory(dreams)

add_library(engine_keen OBJECT KeenEngine.cpp KeenEngine.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine_keen>)
