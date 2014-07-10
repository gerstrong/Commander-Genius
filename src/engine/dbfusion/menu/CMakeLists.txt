
include_directories(${SDL_INCLUDE_DIR})

add_library(dbfusion_menu OBJECT MainMenu.h MainMenu.cpp
                                 FusionMenu.h FusionMenu.cpp
                                 settingsmenu.h settingsmenu.cpp
                                 videosettings.h videosettings.cpp)


set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:dbfusion_menu>)

