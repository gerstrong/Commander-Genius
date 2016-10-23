
include_directories(${SDL_INCLUDE_DIR})

if(NOTYPESAVE)
    add_definitions(-DNOTYPESAVE)
endif()

FILE(GLOB_RECURSE srcs *.cpp *.h)
add_library(engine_keen_dreams OBJECT ${srcs})


set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:engine_keen_dreams>)
