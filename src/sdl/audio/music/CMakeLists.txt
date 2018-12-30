
include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_extensions_audio_music CIMFPlayer.cpp CIMFPlayer.h
                                       CMusic.cpp CMusic.h
                                       CMusicPlayer.cpp CMusicPlayer.h)

if(USE_SDL2)
    LINK_DIRECTORIES(${SDL_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio_music
                          ${SDL_MIXER_LIBRARY})
else(USE_SDL2)
    LINK_DIRECTORIES(${SDL2_MIXER_LIBRARY_DIRS})
    target_link_libraries(sdl_extensions_audio_music
                          ${SDL2_MIXER_LIBRARY})
endif(USE_SDL2)

