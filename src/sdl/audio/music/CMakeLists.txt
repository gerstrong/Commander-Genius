
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_music CIMFPlayer.cpp CIMFPlayer.h
                            CMusic.cpp CMusic.h
                            CMusicPlayer.cpp CMusicPlayer.h
		            COGGPlayer.cpp COGGPlayer.h)

add_dependencies(sdl_audio_music sdl_audio)
