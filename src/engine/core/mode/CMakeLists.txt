
include_directories(${SDL_INCLUDE_DIR})

set(SRCS
    CGameMode.h
    CGamePlayMode.cpp
    CGamePlayMode.h
    CPlayGame.cpp
    CPlayGame.h)

add_library(engine_core_mode STATIC ${SRCS})

# TODO: Not sure why Windows requires this yet.
#if(WIN32)
#    Target_link_libraries(engine_core SDL2)
#endif(WIN32)

Target_link_libraries(engine_core_mode GsKit)
