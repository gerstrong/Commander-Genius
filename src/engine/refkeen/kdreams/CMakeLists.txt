include_directories(${SDL_INCLUDE_DIR})

FILE(GLOB INCLUDES *.h)
#FILE(GLOB SOURCES *.c)

add_definitions(-DREFKEEN_VER_KDREAMS)
add_definitions(-DREFKEEN_VER_KDREAMS_ANYEGA_ALL)
add_definitions(-DGRMODE=EGAGR)


add_library(refkeen_kdreams OBJECT 
        id_ca.cpp
        id_in.cpp
        id_mm.cpp
        id_rf_a.cpp
        id_rf.cpp
        id_sd.cpp
        id_us_a.cpp
        id_us.cpp
        id_vw_a.cpp
        id_vw_ae.cpp
        id_vw.cpp
        kd_act1.cpp
        kd_act2.cpp
        kd_demo.cpp
        kd_keen.cpp
        kd_main.cpp
        kd_play.cpp
        statetype_ptr_conversion.cpp
        soft.cpp
        gelib.cpp
        jam_io.cpp
        lscr/loadscn2.cpp
        lzhuf.cpp
        ${INCLUDES}
        )


#ifeq ($(VERDIRNAME), kdreamse)
#INTCXXFLAGS+= -DREFKEEN_VER_KDREAMS_ANYEGA_ALL=1
#EXENAME=refkdreamse
#else
#INTCXXFLAGS+= -DREFKEEN_VER_KDREAMS_CGA_ALL=1 -DREFKEEN_VER_ANY_CGA=1
#EXENAME=refkdreamsc
#endif

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)


set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:refkeen_kdreams>)
