
add_subdirectory(audio)
add_subdirectory(sys)

include_directories(${SDL_INCLUDE_DIR})
add_library(sdl extensions.cpp extensions.h)

target_link_libraries(sdl sdl_audio)
target_link_libraries(sdl sdl_sys)
