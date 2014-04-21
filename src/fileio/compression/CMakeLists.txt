
include_directories(${SDL_INCLUDE_DIR})
add_library(fileio_compression CCarmack.cpp CCarmack.h
                               CHuffman.cpp CHuffman.h
                               CRLE.cpp CRLE.h
                               Cunlzexe.cpp Cunlzexe.h
                               sgrle.cpp)
