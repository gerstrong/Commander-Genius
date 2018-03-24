
include_directories(${SDL_INCLUDE_DIR})

if(NOTYPESAVE)
    add_definitions(-DNOTYPESAVE)
endif()

add_subdirectory(common)
add_subdirectory(ep4)
add_subdirectory(ep5)
add_subdirectory(ep6)
add_subdirectory(menu)
add_subdirectory(res)

set(CXXSRC
	CLevelPlay.cpp
	CMapPlayGalaxy.cpp
	CPassive.cpp
	CPlayGameGalaxy.cpp
	GalaxyEngine.cpp)

set(HSRC
	CLevelPlay.h
	CMapPlayGalaxy.h
	CPassive.h
	CPlayGameGalaxy.h
	GalaxyEngine.h)



add_library(engine_keen_galaxy ${HSRC} ${CXXSRC})

target_link_libraries(engine_keen_galaxy 
                      engine_keen_galaxy_common
                      engine_keen_galaxy_ep4
                      engine_keen_galaxy_ep5
                      engine_keen_galaxy_ep6
                      engine_keen_galaxy_menu
                      engine_keen_galaxy_res
		      engine_keen)

cotire(engine_keen_galaxy)
