
include_directories(${SDL_INCLUDE_DIR})

add_library(engine_core OBJECT CSettings.cpp CSettings.h
                               CResourceLoader.cpp CResourceLoader.h
                               GameEngine.cpp GameEngine.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine_core>)
