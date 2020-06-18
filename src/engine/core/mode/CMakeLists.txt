
include_directories(${SDL_INCLUDE_DIR})

set(SRCS
    CGameMode.h
    CGamePlayMode.cpp
    CGamePlayMode.h
    CPlayGame.cpp
    CPlayGame.h)

add_library(engine_core_mode STATIC ${SRCS})

Target_link_libraries(engine_core_mode GsKit)
