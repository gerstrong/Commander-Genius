/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include "CGUITextSelectionList.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CVideoDriver.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/InputEvents.h"
#include "sdl/input/CInput.h"

const float TEXT_HEIGHT = 10.0f;

CGUITextSelectionList::CGUITextSelectionList() :
mHoverSelection(0),
mSelection(-1),
mScrollPos(0)
{}

void CGUITextSelectionList::setConfirmButtonEvent(CEvent *ev)
{
	mConfirmEvent.reset(ev);
}

void CGUITextSelectionList::setBackButtonEvent(CEvent *ev)
{
	mBackEvent.reset(ev);
}

bool CGUITextSelectionList::sendEvent(const InputCommands command)
{
	if(command == IC_UP)
	{
		mSelection--;

		if(mSelection < 0)
			mSelection = mItemList.size()-1;
		return true;
	}
	else if(command == IC_DOWN)
	{
		mSelection++;

		if(mSelection >= static_cast<int>(mItemList.size()) )
			mSelection = 0;
		return true;
	}
	else if(command == IC_STATUS || command == IC_JUMP ||
			 command == IC_POGO || command == IC_FIRE)
	{
		if(mConfirmEvent)
			g_pBehaviorEngine->m_EventList.add(mConfirmEvent);
		return true;
	}
	else if(command == IC_BACK)
	{
		if(mBackEvent)
			g_pBehaviorEngine->m_EventList.add(mBackEvent);
		return true;
	}
	else
		return false;

}

void CGUITextSelectionList::addText(const std::string &text)
{
	mItemList.push_back(text);
}

void CGUITextSelectionList::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on something!!

    const float bw = g_pVideoDriver->getGameResolution().w;
	const float bh = g_pVideoDriver->getGameResolution().h;

    const float fx = mRect.x;
	const float fw = mRect.w;
	const float fy = mRect.y;
	const float fh = mRect.h;

    const float y_innerbound_min = fy + static_cast<float>(TEXT_HEIGHT)/bh;
    const float y_innerbound_max = y_innerbound_min +
            static_cast<float>( mItemList.size()*TEXT_HEIGHT )/bh;

    const float x_innerbound_min = fx + static_cast<float>(TEXT_HEIGHT)/bw;
    /*const float x_innerbound_max = x_innerbound_min +
                                    static_cast<float>(10)/bw;*/

	CRect<float> rRect(fx, fy, fw, fh);

	if( MouseMoveEvent *mouseevent = g_pInput->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( rRect.HasPoint(MousePos) )
		{
			if( MousePos.y > y_innerbound_min && MousePos.y < y_innerbound_max )
			{
				int newselection = ((MousePos.y-fy)*bh/TEXT_HEIGHT) - 1;

                if( MousePos.x > x_innerbound_min)
                {
                    if(mouseevent->Type == MOUSEEVENT_MOVED)
                        mHoverSelection = newselection;
                    else if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
                        mSelection = newselection;
                }
			}

			g_pInput->m_EventList.pop_Event();
		}
	}
}

void CGUITextSelectionList::drawScrollBar(const SDL_Rect &lRect)
{
    SDL_Rect scrollRect = lRect;
    SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

    scrollRect.x += 1;
    scrollRect.y += 1;
    scrollRect.w  = 10;
    scrollRect.h -= 2;

    SDL_Rect bDownRect = scrollRect;

    bDownRect.x += 1;
    bDownRect.y += 1;
    bDownRect.w  = 8;
    bDownRect.h  = 8;

    SDL_Rect bUpRect = bDownRect;

    bUpRect.y = (scrollRect.y+scrollRect.h) - (bDownRect.h+1);

    SDL_FillRect(Blitsurface, &scrollRect, 0xFFAFAFAF);
    SDL_FillRect(Blitsurface, &bUpRect,    0xFF4F4F4F);
    SDL_FillRect(Blitsurface, &bDownRect,  0xFF4F4F4F);
}

void CGUITextSelectionList::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

	SDL_Rect lRect = displayRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0xFFFFFFFF);

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
    const int sepHeight = Font.getPixelTextHeight()+2;
    const int xpos = lRect.x+16+1;
    const int ypos = lRect.y+sepHeight;
	unsigned int textlimitWidth = (lRect.w-16)/8;

    const unsigned int lastToShow = (lRect.h/sepHeight)-1;

	lRect.h = 10;
    lRect.x += 12;
    lRect.w -= 12;
	std::string trimmedText;
	std::list<std::string> :: iterator it = mItemList.begin();

    for ( unsigned int line = 0; line<lastToShow ; it++, line++ )
	{
        if(mSelection == (int)line)
		{
            lRect.y = ypos+(line*sepHeight)-1;
			SDL_FillRect(Blitsurface, &lRect, 0xFFC5C5F1);
		}

		trimmedText = *it;
		if(trimmedText.size() > textlimitWidth)
			trimmedText = trimmedText.substr(0, textlimitWidth);

		Font.drawFont(Blitsurface, trimmedText, xpos, ypos+(line*10), false);
	}

    // Do we need a scrollbar?
    if(lastToShow<mItemList.size())
    {
        drawScrollBar(displayRect.SDLRect());
    }


}
