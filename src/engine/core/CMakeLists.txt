
include_directories(${SDL_INCLUDE_DIR})
add_library(engine_core CSettings.cpp CSettings.h
                        CResourceLoader.cpp CResourceLoader.h
                        GameEngine.cpp GameEngine.h)
