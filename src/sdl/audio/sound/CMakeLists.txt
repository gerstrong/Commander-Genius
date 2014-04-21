
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_sound CSoundChannel.cpp CSoundChannel.h
                            CSoundSlot.cpp CSoundSlot.h)

