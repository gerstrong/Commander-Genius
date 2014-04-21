
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})

add_library(fileio CPatcher.cpp CPatcher.h
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

target_link_libraries(fileio fileio_compression)
