/*
 * CGUIBitmap.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#include "CGUIBitmap.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"

CGUIBitmap::CGUIBitmap(std::unique_ptr<CBitmap>&& pBitmap) :
mpBitmap(move(pBitmap))
{}



CGUIBitmap::CGUIBitmap(const std::string &text) :
mTextID(text)
{
    updateGraphics();
}


void CGUIBitmap::updateGraphics()
{
    CRect<Uint16> sizeForScreen = g_pVideoDriver->getGameResolution();
    mpBitmap.reset(new CBitmap( *g_pGfxEngine->getBitmapFromStr(mTextID) ));

    sizeForScreen.w /= 320;
    sizeForScreen.h /= 200;
    sizeForScreen.w *= mpBitmap->getWidth();
    sizeForScreen.h *= mpBitmap->getHeight();

    mpBitmap->scaleTo(sizeForScreen);
}


void CGUIBitmap::processLogic()
{

}



void CGUIBitmap::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	mpBitmap->draw( lRect.x, lRect.y );
}

