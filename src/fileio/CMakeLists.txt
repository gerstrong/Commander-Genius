
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})

add_library(fileio OBJECT CPatcher.cpp CPatcher.h
                          CPatcherUtils.cpp
                          crc.cpp crc.h
                          CSavedGameCoder.h
                          CSaveGameController.h
                   CTileLoader.cpp CTileLoader.h
                   CTileProperties.cpp CTileProperties.h
                   #KeenFiles.cpp KeenFiles.h
                   CExeFile.cpp CExeFile.h
                   lz.cpp lz.h
                   Oldsavegamestructs.h
                   TypeDefinitions.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:fileio>)
