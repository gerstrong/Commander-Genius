
option(NOTYPESAVE "Enable this if you don't want to type names for your savesgames" no)

include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(core)
add_subdirectory(keen)
add_subdirectory(refkeen)

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

add_library(engine ${CXXSRC}
                   ${HSRC})

target_link_libraries(engine 
                      refkeen 
                      engine_keen_galaxy  
		      engine_keen_vorticon
		      engine_keen_dreams
                      engine_core                       
                      cg_dialog
                      unzip)

TARGET_LINK_LIBRARIES(engine ${CURL_LIBRARIES})
