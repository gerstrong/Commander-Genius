
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(core)
add_subdirectory(keen)

add_library(engine OBJECT CGameLauncher.cpp CGameLauncher.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine>)
