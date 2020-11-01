
include_directories(${SDL_INCLUDE_DIR})

add_library(sdl_extensions_audio_sound STATIC CSoundChannel.cpp CSoundChannel.h
                                              CSoundSlot.cpp CSoundSlot.h)

target_link_libraries(sdl_extensions_audio_sound SDL2_mixer)
