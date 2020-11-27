cmake_minimum_required(VERSION 3.5)

Project(GsKit_graphics)

set(HEADERS cgttf.h   GsFont.h     GsGraphics.h    GsSurface.h
    GsTrueTypeFont.h  GsBitmap.h   GsFontLegacy.h  GsPalette.h
    GsScrollsurface.h
    GsTexture.h       GsCursor.h   GsGametext.h    GsSprite.h
    GsTilemap.h       GsDynColor.h CColorMerge.h   CDimDark.h
    GsScrollsurface.h
    CFlash.h          CPixelate.h    CScrollEffect.h    CVibrate.h)

set(SRCS cgttf.cpp     GsFontLegacy.cpp  GsSprite.cpp   GsTrueTypeFont.cpp
    GsBitmap.cpp       GsGametext.cpp    GsSurface.cpp
    GsCursor.cpp       GsGraphics.cpp    GsTexture.cpp
    GsScrollsurface.cpp
    GsFont.cpp         GsPalette.cpp     GsTilemap.cpp GsDynColor.cpp
    CColorMerge.cpp    CDimDark.cpp      CPixelate.cpp CFlash.cpp
    GsScrollsurface.cpp
    CScrollEffect.cpp  CVibrate.cpp  )


#include_directories(${SDL_INCLUDE_DIR})
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
