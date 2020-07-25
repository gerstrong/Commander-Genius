cmake_minimum_required(VERSION 3.5)

Project(GsKit_graphics)

set(HEADERS cgttf.h     GsFont.h        GsGraphics.h  GsSurface.h  GsTrueTypeFont.h
    GsBitmap.h  GsFontLegacy.h  GsPalette.h   GsTexture.h
    GsCursor.h  GsGametext.h    GsSprite.h    GsTilemap.h GsDynColor.h)

set(SRCS cgttf.cpp     GsFontLegacy.cpp  GsSprite.cpp   GsTrueTypeFont.cpp
    GsBitmap.cpp  GsGametext.cpp    GsSurface.cpp
    GsCursor.cpp  GsGraphics.cpp    GsTexture.cpp
    GsFont.cpp    GsPalette.cpp     GsTilemap.cpp GsDynColor.cpp)
    
add_library(${PROJECT_NAME} ${HEADERS} ${SRCS})
