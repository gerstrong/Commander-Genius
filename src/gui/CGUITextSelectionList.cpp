/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUITextSelectionList.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"


void CGUITextSelectionList::addText(const std::string &text)
{
	mItemList.push_back(text);
}

void CGUITextSelectionList::processLogic()
{


}

void CGUITextSelectionList::processRender()
{
	// Blit the List surface
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	SDL_Rect lRect = mRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0x00FFFFFF);

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
	int xpos = mRect.x+16+1;
	std::list<std::string> :: iterator it = mItemList.begin();
	for ( int ypos = 1; it != mItemList.end() ; it++, ypos+=10 )
	{
		Font.drawFont(g_pVideoDriver->getBlitSurface(), *it, xpos, mRect.y+ypos, false);
	}

}
