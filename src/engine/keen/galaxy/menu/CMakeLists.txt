set(CXXSRC 
CConfirmMenu.cpp
ComputerWrist.cpp
GalaxyMenu.cpp
)

set(HSRC
CConfirmMenu.h
CHelpMenu.h
ComputerWrist.h
GalaxyMenu.h
)

add_library(engine_keen_galaxy_menu ${CXXSRC} ${HSRC} )
