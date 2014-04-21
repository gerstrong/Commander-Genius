
add_subdirectory(base)
add_subdirectory(music)
add_subdirectory(sound)

include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_audio Audio.cpp Audio.h
                      Mixer.cpp
                      CAudioResources.cpp CAudioResources.h)

target_link_libraries(sdl_audio sdl_audio_music
                                sdl_audio_sound
                                sdl_audio_base)




