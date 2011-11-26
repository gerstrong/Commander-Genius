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

const float TEXT_HEIGHT = 10.0f;

void CGUITextSelectionList::addText(const std::string &text)
{
	mItemList.push_back(text);
}

void CGUITextSelectionList::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on something!!

	const float bw = g_pVideoDriver->getGameResolution().w;
	const float bh = g_pVideoDriver->getGameResolution().h;

	const float fx = mRect.x/bw;
	const float fw = mRect.w/bw;
	const float fy = mRect.y/bh;
	const float fh = mRect.h/bh;

	const float y_innerbound_min = fy + static_cast<float>(TEXT_HEIGHT)/bh;
	const float y_innerbound_max = y_innerbound_min +
			static_cast<float>( mItemList.size()*TEXT_HEIGHT )/bh;

	CRect<float> rRect(fx, fy, fw, fh);

	if( MouseMoveEvent *mouseevent = g_pBehaviorEngine->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( rRect.HasPoint(MousePos) )
		{
			if( MousePos.y > y_innerbound_min && MousePos.y < y_innerbound_max )
			{
				int newselection = ((MousePos.y-fy)*bh/TEXT_HEIGHT) - 1;

				if(mouseevent->Type == MOUSEEVENT_MOVED)
					mHoverSelection = newselection;
				else if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
					mSelection = newselection;
			}
		}

		g_pBehaviorEngine->m_EventList.pop_Event();
	}
}

void CGUITextSelectionList::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

	SDL_Rect lRect = displayRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0x00FFFFFF);

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
	int xpos = lRect.x+16+1;
	std::list<std::string> :: iterator it = mItemList.begin();
	for ( int line = 0; it != mItemList.end() ; it++, line++ )
	{
		if(mSelection == line)
			Font.drawFont(Blitsurface, *it, xpos, lRect.y+10+(line*10), true);
		else
			Font.drawFont(Blitsurface, *it, xpos, lRect.y+10+(line*10), false);
	}

}
