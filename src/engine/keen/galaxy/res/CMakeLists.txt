set(CXXSRC 
CAudioGalaxy.cpp
CEGAGraphicsGalaxy.cpp
GraphicsIDs.cpp
ItemsGalaxy.cpp
)

set(HSRC
CAudioGalaxy.h
CEGAGraphicsGalaxy.h
EGAStructs.h
ItemsGalaxy.h
)

add_library(engine_keen_galaxy_res ${CXXSRC} ${HSRC} )
