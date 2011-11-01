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

void CGUIText::processRender()
{
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFontCentered(g_pVideoDriver->getBlitSurface(), mText, mRect.x, mRect.w, mRect.y);
}
