
include_directories(${SDL_INCLUDE_DIR})

add_library(sdl_extensions_audio_sound STATIC CSoundChannel.cpp CSoundChannel.h
                                              CSoundSlot.cpp CSoundSlot.h)

if(USE_SDL2)
    LINK_DIRECTORIES(${SDL_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio_sound
                          ${SDL_MIXER_LIBRARY})
else(USE_SDL2)
    LINK_DIRECTORIES(${SDL2_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio_sound
                          ${SDL2_MIXER_LIBRARY})
endif(USE_SDL2)
