
add_subdirectory(audio)
add_subdirectory(sys)

include_directories(${SDL_INCLUDE_DIR})

add_library(sdl OBJECT extensions.cpp extensions.h)
set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:sdl>)

#message("$<TARGET_OBJECTS:sdl_audio>")

#target_link_libraries(sdl sdl_audio)
#target_link_libraries(sdl sdl_sys)
