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
add_subdirectory(depklite)



set(CXXSRC CGameLauncher.cpp
           downloadgui.cpp
           gamedownloader.cpp)


set(HSRC CGameLauncher.h
         gamedownloader.h)

add_library(engine STATIC ${CXXSRC}
                          ${HSRC}
                          gamelauncher.menu)

if(DOWNLOADER)
    target_compile_definitions(engine PRIVATE -DDOWNLOADER)

    FIND_PACKAGE(CURL REQUIRED)

    if(CURL_FOUND)
      message(STATUS "Found curl version ${CURL_VERSION_STRING}")
      if(${CURL_VERSION_STRING} VERSION_LESS "7.32")
        message(FATAL_ERROR "curl version must be at least 7.32")
      endif()
      if(WIN32)
        message(STATIC "Enforcing the use of curl as static library")
        target_compile_definitions(engine -DCURL_STATICLIB)
      endif()
        target_include_directories(engine PRIVATE ${CURL_INCLUDE_DIR})
    else()
      message(FATAL_ERROR "curl library not found")
    endif()

endif()


if (NOT OSXCROSS)
    add_executable(xxd xxd.c)
endif()

add_custom_target(
  createGameLauncherMenu
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND xxd -i gamelauncher.menu ${CMAKE_CURRENT_BINARY_DIR}/gamelauncher.menu.h
)

add_dependencies(engine createGameLauncherMenu)

target_include_directories(engine PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

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
                      unzip
                      depklite)

TARGET_LINK_LIBRARIES(engine ${CURL_LIBRARIES})
