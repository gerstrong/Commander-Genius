set(CXXSRC 
CMapLoaderGalaxyEp6.cpp
ai/CBabobba.cpp
ai/CBip.cpp
ai/CBipShip.cpp
ai/CBloog.cpp
ai/CBlooglet.cpp
ai/CBlooguard.cpp
ai/CBlorb.cpp
ai/CBobba.cpp
ai/CCeilick.cpp
ai/CFlect.cpp
ai/CFleex.cpp
ai/CGik.cpp
ai/CGrabbiter.cpp
ai/CMolly.cpp
ai/CNospike.cpp
ai/COrbatrix.cpp
ai/CRocket.cpp
ai/CRope.cpp
ai/CSatelite.cpp
ai/CSpecialItem.cpp
)

set(HSRC
CMapLoaderGalaxyEp6.h
ai/CBabobba.h
ai/CBip.h
ai/CBipShip.h
ai/CBloog.h
ai/CBlooglet.h
ai/CBlooguard.h
ai/CBlorb.h
ai/CBobba.h
ai/CCeilick.h
ai/CFlect.h
ai/CFleex.h
ai/CGik.h
ai/CGrabbiter.h
ai/CMolly.h
ai/CNospike.h
ai/COrbatrix.h
ai/CRocket.h
ai/CRope.h
ai/CSatelite.h
ai/CSpecialItem.h
)

add_library(engine_keen_galaxy_ep6 ${CXXSRC} ${HSRC} )


target_link_libraries(engine_keen_galaxy_ep6
                      engine_keen_galaxy_common
                      engine_keen_galaxy_menu
                      engine_keen_galaxy_res)
