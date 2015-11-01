include_directories(${SDL_INCLUDE_DIR})

#FILE(GLOB INCLUDES *.h)
#FILE(GLOB SOURCES *.c)


add_library(refkeen_id91_11 OBJECT 
        id_ca.c
        id_in.c
        id_mm.c
        id_sd.c
        id_us_a.c
        id_us_1.c
        id_us_2.c
        id_vw_a.c
        id_vw_ae.c
        id_vw.c)

#ifeq ($(VERDIRNAME), kdreamse)
#OBJECTS+= $(OBJ)/id_vw_ae.o \
#          $(OBJ)/gelib.o \
#          $(OBJ)/jam_io.o \
#          $(OBJ)/loadscn2.o \
#          $(OBJ)/lzhuf.o \
#          $(OBJ)/soft.o
#else
#OBJECTS+= $(OBJ)/id_vw_ac.o
#endif

#ifeq ($(VERDIRNAME), kdreamse)
#INTCXXFLAGS+= -DREFKEEN_VER_KDREAMS_ANYEGA_ALL=1
#EXENAME=refkdreamse
#else
#INTCXXFLAGS+= -DREFKEEN_VER_KDREAMS_CGA_ALL=1 -DREFKEEN_VER_ANY_CGA=1
#EXENAME=refkdreamsc
#endif

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/cat_all)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../)


set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:refkeen_id91_11>)
