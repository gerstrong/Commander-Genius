/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUITextSelectionList.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/InputEvents.h"
#include "common/CBehaviorEngine.h"


void CGUITextSelectionList::addText(const std::string &text)
{
	mItemList.push_back(text);
}

void CGUITextSelectionList::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on something!!

	if( MouseMoveEvent *mouseevent = g_pBehaviorEngine->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		const float bw = g_pVideoDriver->getGameResolution().w;
		const float bh = g_pVideoDriver->getGameResolution().h;

		const float fx = mRect.x/bw;
		const float fw = mRect.w/bw;
		const float fy = mRect.y/bh;
		const float fh = mRect.h/bh;

		CRect<float> rRect(fx, fy, fw, fh);
		/*if( rRect.HasPoint(mouseevent->Pos) )
			printf("It's in!\n");
		else
			printf("It's out!\n");

		printf("Rect (%f, %f, %f, %f) - Coord (%f, %f)\n", rRect.x, rRect.y, rRect.w, rRect.h,
							mouseevent->Pos.x, mouseevent->Pos.y);*/


		g_pBehaviorEngine->m_EventList.pop_Event();
	}

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
		Font.drawFont(Blitsurface, *it, xpos, mRect.y+ypos, false);
	}

}
