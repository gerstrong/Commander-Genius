include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB INCLUDES *.h)
FILE(GLOB SOURCES *.c *.cpp)

#FILE(GLOB SOURCES_COMPAT compat/*.c)

#FILE(GLOB INCLUDES_CRC32 crc32/*.h)
#FILE(GLOB SOURCES_CRC32 crc32/*.c)

add_subdirectory(kdreams)

#FILE(GLOB INCLUDES_OPL *.h)
#FILE(GLOB SOURCES_OPL *.c)

#FILE(GLOB INCLUDES_UNLZEXE unlzexe/*.h)
#FILE(GLOB SOURCES_UNLZEXE unlzexe/*.c)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/kdreams)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DREFKEEN_VER_KDREAMS)
add_definitions(-DREFKEEN_VER_KDREAMS_ANYEGA_ALL)
add_definitions(-DGRMODE=EGAGR)


ADD_DEFINITIONS(-Werror=cast-align)

add_library(refkeen OBJECT ${SOURCES} ${INCLUDES}
                           ${SOURCES_UNLZEXE} ${INCLUDES_UNLZEXE})

#target_link_libraries(refkeen refkeen_kdreams)

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:refkeen>)
