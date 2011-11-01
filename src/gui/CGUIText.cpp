/*
 * CGUIText.cpp
 *
 *  Created on: 30.10.2011
 *      Author: gerstrong
 */

#include "CGUIText.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"

CGUIText::CGUIText(const CRect<float> &lRect, const std::string& text) :
CGUIControl(lRect),
mText(text)
{}


void CGUIText::processLogic()
{


}

void CGUIText::processRender()
{
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFont(g_pVideoDriver->getBlitSurface(), mText, mRect.x, mRect.y, false);
}
