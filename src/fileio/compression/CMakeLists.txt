add_library(fileio_compression STATIC CCarmack.cpp CCarmack.h
                                      CHuffman.cpp CHuffman.h
                                      CRLE.cpp CRLE.h
                                      Cunlzexe.cpp Cunlzexe.h
                                      sgrle.cpp)


target_link_libraries(fileio_compression
    PUBLIC
        GsKit_base_interface
        GsKit_base_audio
)

