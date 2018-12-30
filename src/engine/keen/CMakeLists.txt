
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(galaxy)
add_subdirectory(vorticon)
add_subdirectory(dreams)

add_library(engine_keen KeenEngine.cpp 
                        KeenEngine.h
                        menu/CSelectionMenu.h)

target_link_libraries(engine_keen                      
                      fileio)
