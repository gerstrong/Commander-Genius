include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB INCLUDES *.h)
FILE(GLOB SOURCES *.c)

add_definitions(-DREFKEEN_VER_KDREAMS)

add_library(refkeen_kdreams OBJECT ${SOURCES} ${INCLUDES})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:refkeen_kdreams>)
