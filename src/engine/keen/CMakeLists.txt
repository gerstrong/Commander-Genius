
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(galaxy)
add_subdirectory(vorticon)

if(REFKEEN)
add_subdirectory(dreams)
endif()

add_library(engine_keen OBJECT KeenEngine.cpp KeenEngine.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine_keen>)
