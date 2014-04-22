
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_music OBJECT CIMFPlayer.cpp CIMFPlayer.h
                            CMusic.cpp CMusic.h
                            CMusicPlayer.cpp CMusicPlayer.h
		            COGGPlayer.cpp COGGPlayer.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl_audio_music>)
