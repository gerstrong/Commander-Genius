#include_directories(${SDL_INCLUDE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(CSRC actor.c
         actor_collision.c
         actor_toss.c
         actor_worktype.c
         b800.c
         backdrop.c
         cartoon.c
         config.c
         demo.c
         dialog.c
         effects.c
         font.c
         fullscreen_image.c
         game.c
         high_scores.c
         input.c
         main.c
         map.c
         palette.c
         platforms.c
         player.c
         save.c
         status.c
         tile.c
         util.c
         video.c
         files/file.c
         files/vol.c
         sound/audio.c
         sound/music.c
         sound/opl.c
         sound/sfx.c)

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
                                 ${CSRC}
                                 ${HSRC})

target_link_libraries(cosmos_engine engine_core)

