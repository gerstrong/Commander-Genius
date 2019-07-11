
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(widgets)

set(SRCS
    AudioSettings.cpp
    AudioSettings.h
    cheatsmenu.cpp
    cheatsmenu.h
    ControlSettings.cpp
    ControlSettings.h
    CProfilesMenu.cpp
    CProfilesMenu.h
    CVideoSettings.cpp
    CVideoSettings.h
    CHelpMenu.cpp
    CHelpMenu.h
    DisplaySettings.cpp
    DisplaySettings.h
    GameMenu.cpp
    GameMenu.h
    LoadMenu.cpp
    LoadMenu.h
    MainMenu.cpp
    MainMenu.h
    Options.cpp
    Options.h
    SaveMenu.cpp
    SaveMenu.h
    selectionmenu.cpp
    SelectionMenu.h
    SettingsMenu.cpp
    SettingsMenu.h
    VpadSettings.cpp
    VpadSettings.h
)

add_library(engine_core_menu ${SRCS})

Target_link_libraries(engine_core_menu engine_core_menu_widgets)

# TODO: Not sure why Windows requires this yet.
#if(WIN32)
#    Target_link_libraries(engine_core SDL2)
#endif(WIN32)

Target_link_libraries(engine_core_menu GsKit)
