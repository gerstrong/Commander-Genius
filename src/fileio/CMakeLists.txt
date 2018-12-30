
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})

add_library(fileio CExeFile.cpp
    CPatcher.cpp
    CPatcherUtils.cpp
    crc.cpp
    CSaveGameController.cpp
    CTileLoader.cpp
    KeenFiles.cpp
    lz.cpp
    CExeFile.h
    CPatcher.h
    crc.h
    CSaveGameController.h
    CTileLoader.h
    CTileProperties.h
    KeenFiles.h
    lz.h
    Oldsavegamestructs.h)

target_link_libraries(fileio fileio_compression)
