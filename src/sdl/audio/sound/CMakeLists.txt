
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_sound OBJECT CSoundChannel.cpp CSoundChannel.h
                            CSoundSlot.cpp CSoundSlot.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl_audio_sound>)
