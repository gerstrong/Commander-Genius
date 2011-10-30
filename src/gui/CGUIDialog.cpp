/*
 * CGUIDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 */

#include "CGUIDialog.h"
#include "sdl/CVideoDriver.h"


CGUIDialog::CGUIDialog(const CRect &NewRect) :
mRect(NewRect)
{}


void CGUIDialog::processLogic()
{
	g_pVideoDriver->mDr
}


void CGUIDialog::processRendering()
{
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_FillRect(Blitsurface, &mRect, 0x0);
}
