/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <lib/base/PointDevice.h>


#include "GsTextSelectionList.h"

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
        mReleasedSelection--;

        if(mReleasedSelection < 0)
        {
            mReleasedSelection = 0;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection < mScrollbar.scrollPos() )
        {
            mScrollbar.scrollUp();
        }

		return true;
	}
	else if(command == IC_DOWN)
	{
        const int last = static_cast<int>(mItemList.size());

        mReleasedSelection++;

        if(mReleasedSelection >= last)
        {
            mReleasedSelection = last-1;
            mSelected = false;
            return false;
        }

        if( mReleasedSelection >= mScrollbar.lastToShow() - mScrollbar.scrollPos() )
        {
            mScrollbar.scrollDown();
        }

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

    const float bw = gVideoDriver.getGameResolution().w;
	const float bh = gVideoDriver.getGameResolution().h;

	const float fx = mRect.x;
	const float fw = mRect.w;
	const float fy = mRect.y;
	const float fh = mRect.h;

	const float y_innerbound_min = fy + static_cast<float>(TEXT_HEIGHT)/bh;
	const float y_innerbound_max = y_innerbound_min +
			static_cast<float>( mItemList.size()*TEXT_HEIGHT )/bh;

    const float x_innerbound_min = fx + static_cast<float>(TEXT_HEIGHT)/bw;

    GsRect<float> rRect(fx, fy, fw, fh);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState();        

    CVec MousePos = pointingState.mPos;

    if( rRect.HasPoint(MousePos) )
    {

        if( MousePos.y > fy && MousePos.y < y_innerbound_max )
        {
            int newselection = ((MousePos.y-fy)*bh/TEXT_HEIGHT) - 1 + mScrollbar.scrollPos();

            if( MousePos.x > x_innerbound_min && MousePos.y > y_innerbound_min)
            {
                if(mHovered)
                    mHoverSelection = newselection;
                if(mPressed)
                    mPressedSelection = newselection;
                if(mReleased)
                    mReleasedSelection = newselection;
            }
            else if(mReleased)// If clicked on scroll bar
            {
                const float midPart_y = (fy+y_innerbound_max)/2.0;

                if(MousePos.y < midPart_y) // Go up!
                {
                    mScrollbar.scrollUp();
                }
                else if(MousePos.y > midPart_y) // Go down!
                {
                    mScrollbar.scrollDown();
                }
            }
        }
    }
    else
    {
        mHoverSelection = -1;
        mPressedSelection = -1;
    }
}

void CGUITextSelectionList::drawScrollBar(const SDL_Rect &lRect)
{
    SDL_Rect scrollRect = lRect;
    SDL_Surface *Blitsurface = gVideoDriver.getBlitSurface();

    scrollRect.x += 1;
    scrollRect.y += 1;
    scrollRect.w  = 10;
    scrollRect.h -= 2;

    SDL_Rect bScUpRect = scrollRect;
    bScUpRect.w  = 10;
    bScUpRect.h  = 10;

    SDL_Rect bScDownRect = bScUpRect;

    bScDownRect.y = (scrollRect.y+scrollRect.h) - (bScUpRect.h);

    SDL_FillRect(Blitsurface, &scrollRect, 0xFFBFBFBF);

    // Now show the slider
    float relPos = float(mScrollbar.scrollPos()) / float(mScrollbar.mMaxScrollAmt);
    const int posSpace = int(relPos * float(scrollRect.h - (3*10))) + 1;
    SDL_Rect bSliderRect = bScDownRect;
    bSliderRect.x++;
    bSliderRect.y = (bScUpRect.y + bScUpRect.h) + posSpace;
    bSliderRect.w = 8;
    bSliderRect.h = 8;
    SDL_FillRect(Blitsurface, &bSliderRect, 0xFF2F2F2F);

    const Uint32 scButtonColor = 0xFF7F7F7F;

    // Set the up and down arrows
    GsFont &Font = gGraphics.getFont(mFontID);
    SDL_FillRect(Blitsurface, &bScUpRect,   scButtonColor);
    Font.drawFont(Blitsurface, "\017", bScUpRect.x+1, bScUpRect.y+2, false);
    SDL_FillRect(Blitsurface, &bScDownRect, scButtonColor);
    Font.drawFont(Blitsurface, "\023", bScDownRect.x+1, bScDownRect.y, false);
}



void CGUITextSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
	SDL_Surface *Blitsurface = gVideoDriver.getBlitSurface();

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

	SDL_Rect lRect = displayRect.SDLRect();
	SDL_FillRect(Blitsurface, &lRect, 0xFFFFFFFF);

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
    const int sepHeight = Font.getPixelTextHeight()+2;
	const int xpos = lRect.x+16+1;
	const int ypos = lRect.y+10;
	unsigned int textlimitWidth = (lRect.w-16)/8;

    mScrollbar.mLastToShow = (lRect.h/sepHeight)-1;

	lRect.h = 10;
    lRect.x += 12;
    lRect.w -= 12;
	std::string trimmedText;
	std::list<std::string> :: iterator it = mItemList.begin();

    for(int i=0 ; i<mScrollbar.scrollPos() ; it++, i++);

    for ( int line = 0;  it != mItemList.end() && line<mScrollbar.mLastToShow ; it++, line++ )
	{
        if(mPressedSelection == int(line) + mScrollbar.scrollPos() )
        {
            lRect.y = ypos+(line*10)-1;
            SDL_FillRect(Blitsurface, &lRect, 0xFFA5A5F1);
        }
        else if(mReleasedSelection == int(line) + mScrollbar.scrollPos() )
		{
            lRect.y = ypos+(line*10)-1;

            if(mSelected)
                SDL_FillRect(Blitsurface, &lRect, 0xFFB5B5F1);
            else
                SDL_FillRect(Blitsurface, &lRect, 0xFFC5C5C5);
		}
        else if(mHoverSelection == int(line) + mScrollbar.scrollPos() )
        {
            lRect.y = ypos+(line*sepHeight)-1;
            SDL_FillRect(Blitsurface, &lRect, 0xFFE5E5F1);
        }


		trimmedText = *it;
		if(trimmedText.size() > textlimitWidth)
			trimmedText = trimmedText.substr(0, textlimitWidth);

		Font.drawFont(Blitsurface, trimmedText, xpos, ypos+(line*10), false);
	}

    mScrollbar.mMaxScrollAmt = mItemList.size()-mScrollbar.lastToShow();

    // Do we need a scrollbar?
    if(mScrollbar.mMaxScrollAmt>0)
    {
        drawScrollBar(displayRect.SDLRect());
    }

}
