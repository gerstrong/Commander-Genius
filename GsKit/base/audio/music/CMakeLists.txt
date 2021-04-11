
include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_extensions_audio_music STATIC CMusic.cpp CMusic.h
                                              CIMFPlayer.cpp  CIMFPlayer.h
                                              CMusicPlayer.cpp CMusicPlayer.h)

LINK_DIRECTORIES(${SDL_MIXER_LIBRARY_DIRS})
target_link_libraries(sdl_extensions_audio_music
                      ${SDL_MIXER_LIBRARY})

