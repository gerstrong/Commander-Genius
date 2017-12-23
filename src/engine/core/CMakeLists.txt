
include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB_RECURSE srcs *.cpp *.h)

add_library(engine_core ${srcs})
