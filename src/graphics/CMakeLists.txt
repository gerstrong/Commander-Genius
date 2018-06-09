
include_directories(${SDL_INCLUDE_DIR})

SET(SRCS    effects/CColorMerge.cpp
            effects/CColorMerge.h
            effects/CDimDark.cpp
            effects/CDimDark.h
            effects/CFlash.cpp
            effects/CFlash.h
            effects/CPixelate.cpp
            effects/CPixelate.h
            effects/CScrollEffect.cpp
            effects/CScrollEffect.h
            effects/CVibrate.cpp
            effects/CVibrate.h)


add_library(graphics OBJECT ${SRCS})

set_property(GLOBAL APPEND PROPERTY CG_OBJ_LIBS $<TARGET_OBJECTS:graphics>)

