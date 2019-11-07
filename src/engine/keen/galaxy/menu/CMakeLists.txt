set(CXXSRC 
ComputerWrist.cpp
GalaxyMenu.cpp
)

set(HSRC
CHelpMenu.h
ComputerWrist.h
GalaxyMenu.h
)

add_library(engine_keen_galaxy_menu STATIC ${CXXSRC} ${HSRC} )
