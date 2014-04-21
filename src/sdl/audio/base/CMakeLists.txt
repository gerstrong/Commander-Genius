
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_audio_base COPLEmulator.cpp COPLEmulator.h
                           dbopl.cpp dbopl.h
                           Sampling.cpp Sampling.h)

