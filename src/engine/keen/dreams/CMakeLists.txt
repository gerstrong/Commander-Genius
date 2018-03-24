
include_directories(${SDL_INCLUDE_DIR})

if(NOTYPESAVE)
    add_definitions(-DNOTYPESAVE)
endif()


set(CXXSRC
    dreamscontrolpanel.cpp
    dreamsdosintro.cpp
    dreamsengine.cpp
    dreamsgameplay.cpp
    dreamsintro.cpp
    )

set(HSRC
    dreamscontrolpanel.h
    dreamsdosintro.h
    dreamsengine.h
    dreamsgameplay.h
    dreamsintro.h
    )

add_library(engine_keen_dreams ${CXXSRC} ${HSRC})

target_link_libraries(engine_keen_dreams refkeen engine_keen)
