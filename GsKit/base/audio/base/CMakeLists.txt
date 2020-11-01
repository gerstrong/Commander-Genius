
include_directories(${SDL_INCLUDE_DIR})
add_library(sdl_extensions_audio_base STATIC COPLEmulator.cpp COPLEmulator.h
                                             dbopl.cpp dbopl.h
                                             Sampling.cpp Sampling.h)
