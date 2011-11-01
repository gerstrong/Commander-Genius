/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUITextSelectionList.h"
#include "sdl/CVideoDriver.h"

CGUITextSelectionList::CGUITextSelectionList()
{
	// TODO Auto-generated constructor stub

}

void CGUITextSelectionList::processLogic()
{


}

void CGUITextSelectionList::processRender()
{
	//CFont &Font = g_pGfxEngine->getFont(0);

	//Font.drawFont(g_pVideoDriver->getBlitSurface(), mText, mRect.x, mRect.y, false);

	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_Rect lRect = mRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0x00FFFFFF);
}
