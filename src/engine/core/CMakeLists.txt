
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(menu)
add_subdirectory(mode)
add_subdirectory(VGamepads)

set(SRCS ActionFormat.cpp
         CBehaviorEngine.cpp
         CCamera.cpp
         CHUD.cpp
         CIntro.cpp
         CMap.cpp
         CMessages.cpp
         CObjectCollision.cpp
         CPassive.cpp
         CPhysicsSettings.cpp
         CPlane.cpp
         CPlanes.cpp
         CResourceLoader.cpp
         CSettings.cpp
         CSpriteObject.cpp
         GameEngine.cpp
         inventory.cpp
         textmode.cpp
         scrollingplane.cpp


         ActionFormat.h
         CBehaviorEngine.h
         CCamera.h
         CEvent.h
         Cheat.h
         CHUD.h
         CIntro.h
         CMap.h
         CMessages.h
         CPassive.h
         CPhysicsSettings.h
         CPlane.h
         CPlanes.h
         CResourceLoader.h
         CSettings.h
         CSpriteObject.h
         GameEngine.h
         inventory.h
         objenums.h
         options.h
         Playerdefines.h
         spritedefines.h
         textmode.h
         scrollingplane.h)

add_library(engine_core STATIC ${SRCS})

target_link_libraries(engine_core engine_core_menu)
target_link_libraries(engine_core engine_core_mode)
target_link_libraries(engine_core engine_core_vgamepad)

# Link some of our SDL Extensions
if(GP2X)
    target_link_libraries(engine_core sdl_extensions_audio sdl_extensions_sys)
else()
    target_link_libraries(engine_core sdl_extensions_audio)
endif()


Target_link_libraries(engine_core GsKit)
