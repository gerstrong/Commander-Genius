include_directories(${SDL_INCLUDE_DIR})

#FILE(GLOB INCLUDES *.h)
#FILE(GLOB SOURCES *.c)

add_definitions(-DREFKEEN_VER_KDREAMS)
add_definitions(-DREFKEEN_VER_KDREAMS_ANYEGA_ALL)
add_definitions(-DGRMODE=EGAGR)


add_library(refkeen_kdreams OBJECT 
        id_ca.c
        id_in.c
        id_mm.c
        id_rf_a.c
        id_rf.c
        id_sd.c
        id_us_a.c
        id_us.c
        id_vw_a.c
        id_vw_ae.c
        id_vw.c
        kd_act1.c
        kd_act2.c
        kd_demo.c
        kd_keen.c
        kd_main.c
        kd_play.c
        statetype_ptr_conversion.c
        soft.c
        gelib.c
        jam_io.c
        lscr/loadscn2.c
        lzhuf.c
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
