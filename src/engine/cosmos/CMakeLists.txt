#include_directories(${SDL_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

set(CSRC actor.cpp
         actor_collision.cpp
         actor_toss.cpp
         actor_worktype.cpp
         b800.cpp
         backdrop.cpp
         cartoon.cpp
         config.cpp
         demo.cpp
         dialog.cpp
         effects.cpp
         font.cpp
         fullscreen_image.cpp
         game.cpp
         high_scores.cpp
         input.cpp
         main.cpp
         map.cpp
         palette.cpp
         platforms.cpp
         player.cpp
         save.cpp
         status.cpp
         tile.cpp
         util.cpp
         video.cpp
         files/file.cpp
         files/vol.cpp
         sound/audio.cpp
         sound/music.cpp
         sound/opl.cpp
         sound/sfx.cpp)

set(HSRC CosmoEngine.h
         actor_collision.h
         actor.h
         actor_toss.h
         actor_worktype.h
         b800_font.h
         b800.h
         backdrop.h
         cartoon.h
         config.h
         defines.h
         demo.h
         dialog.h
         effects.h
         font.h
         fullscreen_image.h
         game.h
         high_scores.h
         input.h
         map.h
         palette.h
         platforms.h
         player.h
         save.h
         status.h
         tile.h
         util.h
         video.h)

add_library(cosmos_engine STATIC CosmoEngine.cpp
                                 cosmosintro.cpp
                                 cosmogameplay.cpp
                                 ${CSRC}
                                 ${HSRC})

target_link_libraries(cosmos_engine engine_core)

