
OPTION(DBFUSION "Dosbox Fusion" No)
OPTION(REFKEEN "RefKeen for Keen Dreams" YES)

include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(core)
add_subdirectory(keen)


if(DOWNLOADER)
    add_definitions(-DDOWNLOADER)
endif()


if(DBFUSION)
    add_subdirectory(dbfusion)
    add_definitions(-DDBFUSION)
endif()

add_subdirectory(refkeen)

add_library(engine OBJECT CGameLauncher.cpp CGameLauncher.h
                          gamedownloader.cpp gamedownloader.h)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine>)
