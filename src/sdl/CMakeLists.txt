
add_subdirectory(audio)
add_subdirectory(sys)

include_directories(${SDL_INCLUDE_DIR})

add_library(sdl_extensions extensions.cpp extensions.h)

target_link_libraries(sdl_extensions sdl_extensions_audio sdl_extensions_sys)
