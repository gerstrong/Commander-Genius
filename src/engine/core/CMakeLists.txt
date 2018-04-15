
include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB_RECURSE srcs *.cpp *.h)

add_library(engine_core ${srcs})

target_link_libraries(engine_core sdl_extensions_audio sdl_extensions_sys)

target_link_libraries(engine_core sdl_extensions_audio sdl_extensions_sys)

# TODO: Not sure why Windows requires this yet.
#if(WIN32)
#    Target_link_libraries(engine_core SDL2)
#endif(WIN32)

Target_link_libraries(engine_core GsKit)
