
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB SRCS *.cpp *.h)

add_library(fileio ${SRCS})

target_link_libraries(fileio fileio_compression)
