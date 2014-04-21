
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio Audio.cpp Audio.h
                      Mixer.cpp Sampling.cpp Sampling.h)
