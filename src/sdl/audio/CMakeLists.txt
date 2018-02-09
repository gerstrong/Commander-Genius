

add_subdirectory(base)
add_subdirectory(music)
add_subdirectory(sound)

include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_extensions_audio Audio.cpp Audio.h
                                 Mixer.cpp
                                 CAudioResources.cpp CAudioResources.h)

target_link_libraries(sdl_extensions_audio
                      sdl_extensions_audio_music
                      sdl_extensions_audio_sound
                      sdl_extensions_audio_base)


if(USE_SDL2)
    LINK_DIRECTORIES(${SDL2_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio
                          ${SDL2_MIXER_LIBRARY})
else(USE_SDL2)
    LINK_DIRECTORIES(${SDL_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio
                          ${SDL_MIXER_LIBRARY})
endif(USE_SDL2)

