/*
 * CGUIBitmap.cpp
 *
 *  Created on: 22.04.2012
 *      Author: gerstrong
 */

#include "CGUIBitmap.h"
#include "graphics/CGfxEngine.h"

CGUIBitmap::CGUIBitmap(const std::string &text)
{

	mpBitmap = new CBitmap( *g_pGfxEngine->getBitmap(text) );

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
