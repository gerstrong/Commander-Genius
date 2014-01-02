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

#include <lib/base/PointDevice.h>

const float TEXT_HEIGHT = 10.0f;

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
        mPressedSelection--;

        if(mPressedSelection < 0)
            mPressedSelection = mItemList.size()-1;
		return true;
	}
	else if(command == IC_DOWN)
	{
        mPressedSelection++;

        if(mPressedSelection >= static_cast<int>(mItemList.size()) )
            mPressedSelection = 0;
		return true;
	}
	else if(command == IC_STATUS || command == IC_JUMP ||
			 command == IC_POGO || command == IC_FIRE)
	{
		if(mConfirmEvent)
			gEventManager.add(mConfirmEvent);
		return true;
	}
	else if(command == IC_BACK)
	{
		if(mBackEvent)
			gEventManager.add(mBackEvent);
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

	//const float bw = g_pVideoDriver->getGameResolution().w;
	const float bh = g_pVideoDriver->getGameResolution().h;

	const float fx = mRect.x;
	const float fw = mRect.w;
	const float fy = mRect.y;
	const float fh = mRect.h;

	const float y_innerbound_min = fy + static_cast<float>(TEXT_HEIGHT)/bh;
	const float y_innerbound_max = y_innerbound_min +
			static_cast<float>( mItemList.size()*TEXT_HEIGHT )/bh;

    GsRect<float> rRect(fx, fy, fw, fh);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState();

    CVec MousePos = pointingState.mPos;

    if( rRect.HasPoint(MousePos) )
    {
        if( MousePos.y > y_innerbound_min && MousePos.y < y_innerbound_max )
        {
            int newselection = ((MousePos.y-fy)*bh/TEXT_HEIGHT) - 1;

            if(mHovered)
                mHoverSelection = newselection;
            if(mPressed)
                mPressedSelection = newselection;
            if(mReleased)
                mReleasedSelection = newselection;
        }
    }
    else
    {
        mHoverSelection = -1;
        mPressedSelection = -1;
    }
}

void CGUITextSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
	SDL_Surface *Blitsurface = g_pVideoDriver->getBlitSurface();

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

	SDL_Rect lRect = displayRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0xFFFFFFFF);

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
	const int xpos = lRect.x+16+1;
	const int ypos = lRect.y+10;
	unsigned int textlimitWidth = (lRect.w-16)/8;
	lRect.h = 10;
	std::string trimmedText;
	std::list<std::string> :: iterator it = mItemList.begin();
	for ( int line = 0; it != mItemList.end() ; it++, line++ )
	{
        if(mPressedSelection == line)
        {
            lRect.y = ypos+(line*10)-1;
            SDL_FillRect(Blitsurface, &lRect, 0xFFB5B5F1);
        }
        else if(mReleasedSelection == line)
		{
			lRect.y = ypos+(line*10)-1;
			SDL_FillRect(Blitsurface, &lRect, 0xFFC5C5F1);
		}
        else if(mHoverSelection == line)
        {
            lRect.y = ypos+(line*10)-1;
            SDL_FillRect(Blitsurface, &lRect, 0xFFE5E5F1);
        }


		trimmedText = *it;
		if(trimmedText.size() > textlimitWidth)
			trimmedText = trimmedText.substr(0, textlimitWidth);

		Font.drawFont(Blitsurface, trimmedText, xpos, ypos+(line*10), false);
	}

}
