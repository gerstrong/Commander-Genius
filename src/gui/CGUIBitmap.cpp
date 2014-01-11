/*
 * CGUIBitmap.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#include "CGUIBitmap.h"
#include "graphics/GsGraphics.h"
#include <base/video/CVideoDriver.h>

CGUIBitmap::CGUIBitmap(std::unique_ptr<GsBitmap>&& pBitmap) :
mpBitmap(move(pBitmap))
{}



CGUIBitmap::CGUIBitmap(const std::string &text) :
mTextID(text)
{
    updateGraphics();
}


void CGUIBitmap::updateGraphics()
{
    GsRect<Uint16> sizeForScreen = gVideoDriver.getGameResolution();
    mpBitmap.reset(new GsBitmap( *gGraphics.getBitmapFromStr(mTextID) ));

    sizeForScreen.w /= 320;
    sizeForScreen.h /= 200;
    sizeForScreen.w *= mpBitmap->getWidth();
    sizeForScreen.h *= mpBitmap->getHeight();

    mpBitmap->scaleTo(sizeForScreen);
}


void CGUIBitmap::processLogic()
{

}



void CGUIBitmap::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	mpBitmap->draw( lRect.x, lRect.y );
}

