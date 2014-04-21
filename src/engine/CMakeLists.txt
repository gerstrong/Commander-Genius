
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(core)

add_library(engine CGameLauncher.cpp CGameLauncher.h)
target_link_libraries(engine engine_core)


