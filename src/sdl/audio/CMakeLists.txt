
add_subdirectory(base)
add_subdirectory(music)
add_subdirectory(sound)

include_directories(${SDL_INCLUDE_DIR})


add_library(sdl_audio OBJECT Audio.cpp Audio.h
                             Mixer.cpp
                             CAudioResources.cpp CAudioResources.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl_audio>)
