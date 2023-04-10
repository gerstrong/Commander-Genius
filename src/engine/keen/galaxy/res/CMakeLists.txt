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

add_library(engine_keen_galaxy_res STATIC ${CXXSRC} ${HSRC} )

target_link_libraries(engine_keen_galaxy_res GsKit)
