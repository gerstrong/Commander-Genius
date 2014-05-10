
include_directories(${SDL_INCLUDE_DIR})
add_library(fileio_compression OBJECT CCarmack.cpp CCarmack.h
                               CHuffman.cpp CHuffman.h
                               CRLE.cpp CRLE.h
                               Cunlzexe.cpp Cunlzexe.h
                               sgrle.cpp)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:fileio_compression>)
