
include_directories(${SDL_INCLUDE_DIR})

option(NOTYPESAVE "Enable this if you don't want to type names for your savesgames" no)

if(NOTYPESAVE)
    add_definitions(-DNOTYPESAVE)
endif()


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
    GameSpecSettings.cpp
    GameSpecSettings.h
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
    QuitMenu.cpp
    QuitMenu.h
    Options.cpp
    Options.h
    SaveMenu.cpp
    SaveMenu.h
    selectionmenu.cpp
    SelectionMenu.h
    SettingsMenu.cpp
    SettingsMenu.h
    OverlaySettings.cpp
    OverlaySettings.h
    CameraSettings.h
    CameraSettings.cpp
    achievementmenu.cpp
    achievementmenu.h
)

add_library(engine_core_menu STATIC ${SRCS})

target_compile_features(engine_core_menu PRIVATE cxx_std_17)

Target_link_libraries(engine_core_menu engine_core_menu_widgets)

Target_link_libraries(engine_core_menu GsKit)
