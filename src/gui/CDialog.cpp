/*
 * CDialog.cpp
 *
 *  Created on: 29.10.2011
 *      Author: gerstrong
 */

#include "CDialog.h"

#include "sdl/CVideoDriver.h"

CDialog::CDialog(const CRect &NewRect) :
mRect(NewRect)
{}

void CDialog::process()
{
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_FillRect(Blitsurface, &mRect, 0x0);
}
