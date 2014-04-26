
include_directories(${SDL_INCLUDE_DIR})

add_subdirectory(src)

FILE(GLOB include/*.h)

add_library(dbfusion OBJECT config.h ${INCLUDES})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:dbfusion>)
