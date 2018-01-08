
include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_extensions_audio_music CIMFPlayer.cpp CIMFPlayer.h
                                       CMusic.cpp CMusic.h
                                       CMusicPlayer.cpp CMusicPlayer.h
                                       COGGPlayer.cpp COGGPlayer.h)

