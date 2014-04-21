
add_subdirectory(compression)

include_directories(${SDL_INCLUDE_DIR})
add_library(fileio CExeFile.cpp CExeFile.h
		   CPatcher.cpp CPatcher.h
		   CPatcherUtils.cpp 
		   crc.cpp crc.h
		   CSavedGameCoder.h
                   CSaveGameController.h
                   CTileLoader.cpp CTileLoader.h
                   KeenFiles.cpp KeenFiles.h
                   lz.cpp lz.h
                   Oldsavegamestructs.h
                   TypeDefinitions.h)

target_link_libraries(fileio fileio_compression)
