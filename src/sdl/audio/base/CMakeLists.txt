
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_base OBJECT COPLEmulator.cpp COPLEmulator.h
                           dbopl.cpp dbopl.h
                           Sampling.cpp Sampling.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl_audio_base>)

