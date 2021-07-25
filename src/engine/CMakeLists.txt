include_directories(${SDL_INCLUDE_DIR})

option(BUILD_COSMOS  "Build Cosmos engine" no)
option(BUILD_KEEN    "Build Keen engine" yes)
option(BUILD_REFKEEN "Build RefKeen engine" yes)

add_subdirectory(core)

if(BUILD_KEEN)
    add_subdirectory(keen)
    MESSAGE(STATUS "Keen engine build enabled")
endif(BUILD_KEEN)

if(BUILD_REFKEEN)
    add_subdirectory(refkeen)
    MESSAGE(STATUS "Refkeen engine build enabled")
endif(BUILD_REFKEEN)

if(BUILD_COSMOS)
        add_definitions(-DBUILD_COSMOS)
	add_subdirectory(cosmos)
        MESSAGE(STATUS "Cosmos build enabled")
endif(BUILD_COSMOS)


# Move this to somewhere else
add_subdirectory(unzip)

if(DOWNLOADER)
    add_definitions(-DDOWNLOADER)

    FIND_PACKAGE(CURL)

    IF(CURL_FOUND)
      if(WIN32)
         ADD_DEFINITIONS(-DCURL_STATICLIB)
      endif()
        INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
    ELSE()
      MESSAGE(FATAL_ERROR "Could not find the CURL library and development files." )
    ENDIF()

endif()


set(CXXSRC CGameLauncher.cpp
           downloadgui.cpp
           gamedownloader.cpp)


set(HSRC CGameLauncher.h
         gamedownloader.h)

add_library(engine STATIC ${CXXSRC}
                          ${HSRC})

if(BUILD_KEEN)
  target_link_libraries(engine engine_keen_galaxy engine_keen_vorticon)
endif(BUILD_KEEN)

if(BUILD_REFKEEN)
  target_link_libraries(engine refkeen engine_keen_dreams)
endif(BUILD_REFKEEN)

if(BUILD_COSMOS)
  target_link_libraries(engine cosmos_engine)
endif(BUILD_COSMOS)


target_link_libraries(engine
                      cg_dialog
                      unzip)

TARGET_LINK_LIBRARIES(engine ${CURL_LIBRARIES})
