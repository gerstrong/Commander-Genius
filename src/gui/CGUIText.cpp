/*
 * CGUIText.cpp
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 */

#include "CGUIText.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"

CGUIText::CGUIText(const std::string& text) :
mText(text)
{}


void CGUIText::processLogic()
{

}

void CGUIText::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFontCentered(g_pVideoDriver->getBlitSurface(), mText, lRect.x, lRect.w, lRect.y, false);
}
