set(CXXSRC 
CMapLoaderGalaxyEp5.cpp
ai/CAmpton.cpp
ai/CFuse.cpp
ai/CKorath.cpp
ai/CMastersShot.cpp
ai/CMastersSpark.cpp
ai/CRedShot.cpp
ai/CRoboRed.cpp
ai/CShelly.cpp
ai/CShikadi.cpp
ai/CShikadiMaster.cpp
ai/CShikadiMine.cpp
ai/CShockshound.cpp
ai/CSlicestar.cpp
ai/CSparky.cpp
ai/CSphereful.cpp
ai/CSpindred.cpp
ai/CSpirogrip.cpp
ai/CVolteface.cpp
)

set(HSRC
CMapLoaderGalaxyEp5.h
ai/CAmpton.h
ai/CFuse.h
ai/CKorath.h
ai/CMastersShot.h
ai/CMastersSpark.h
ai/CRedShot.h
ai/CRoboRed.h
ai/CShelly.h
ai/CShikadi.h
ai/CShikadiMaster.h
ai/CShikadiMine.h
ai/CShockshound.h
ai/CSlicestar.h
ai/CSparky.h
ai/CSphereful.h
ai/CSpindred.h
ai/CSpirogrip.h
ai/CVolteface.h
)

add_library(engine_keen_galaxy_ep5 ${CXXSRC} ${HSRC} )


target_link_libraries(engine_keen_galaxy_ep5
                      engine_keen_galaxy_common
                      engine_keen_galaxy_menu
                      engine_keen_galaxy_res)
