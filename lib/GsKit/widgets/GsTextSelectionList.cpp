/*
 * CGUITextSelectionList.cpp
 *
 *  Created on: 01.11.2011
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/PointDevice.h>


#include "GsTextSelectionList.h"


CGUITextSelectionList::CGUITextSelectionList()  :
    mHoverSelection(0),
    mPressedSelection(-1),
    mReleasedSelection(-1),
    mScrollbar(this)
{
    GsRect<float> scrollRect(0.0f, 0.0f, 0.1f, 1.0f);
    mScrollbar.setRect(scrollRect);
}

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
    if(!mEnabled)
        return;

	// Here we check if the mouse-cursor/Touch entry clicked on something!!

    const float bw = gVideoDriver.getGameResolution().w;
	const float bh = gVideoDriver.getGameResolution().h;

	const float fx = mRect.x;
	const float fw = mRect.w;
	const float fy = mRect.y;
	const float fh = mRect.h;

    GsFont &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();

    const float textHeight = (pixth+2);

    const float y_innerbound_min = fy + static_cast<float>(textHeight)/bh;
	const float y_innerbound_max = y_innerbound_min +
            static_cast<float>( mItemList.size()*textHeight )/bh;

    const float x_innerbound_min = fx + static_cast<float>(textHeight)/bw;


    GsRect<float> rRect(fx, fy, fw, fh);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState();        

    Vector2D<float> mousePos = pointingState.mPos;

    if( rRect.HasPoint(mousePos) )
    {
        // Let scrollbar do it's work
        mScrollbar.processLogic();

        if( mousePos.y > fy && mousePos.y < y_innerbound_max )
        {
            int newselection = ((mousePos.y-fy)*bh/textHeight) - 1 + mScrollbar.scrollPos();

            if( mousePos.x > x_innerbound_min && mousePos.y > y_innerbound_min)
            {
                if(mHovered)
                    mHoverSelection = newselection;
                if(mPressed)
                    mPressedSelection = newselection;
                if(mReleased)
                    mReleasedSelection = newselection;
            }
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
    SDL_Surface *pBlitsurface = gVideoDriver.getBlitSurface();

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

	SDL_Rect lRect = displayRect.SDLRect();


    if(!mEnabled)
    {
        SDL_FillRect(pBlitsurface, &lRect, 0xFFDFDFDF);
    }
    else
    {
        SDL_FillRect(pBlitsurface, &lRect, 0xFFFFFFFF);
    }

	// Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();
    const int pixtw = pixth; // NOTE: We assume here, that the height and width are the same. Invalid to galaxy fonts!

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
    const int sepHeight = Font.getPixelTextHeight()+2;
	const int xpos = lRect.x+16+1;
	const int ypos = lRect.y+10;
    unsigned int textlimitWidth = (lRect.w-16)/pixtw;

    mScrollbar.mLastToShow = (lRect.h/sepHeight)-1;

    lRect.h = pixth+2;
    lRect.x += 12;
    lRect.w -= 12;
	std::string trimmedText;
    auto it = mItemList.begin();

    for(int i=0 ; i<mScrollbar.scrollPos() ; it++, i++);

    for ( int line = 0;  it != mItemList.end() && line<mScrollbar.mLastToShow ; it++, line++ )
	{
        // Current line to be rendered
        const int curLinePos = static_cast<int>(line) + mScrollbar.scrollPos();

        if(mPressedSelection == curLinePos )
        {
            lRect.y = ypos+(line*lRect.h)-1;
            SDL_FillRect(pBlitsurface, &lRect, 0xFFA5A5F1);
        }
        else if(mReleasedSelection == curLinePos )
		{
            lRect.y = ypos+(line*lRect.h)-1;

            if(mSelected)
                SDL_FillRect(pBlitsurface, &lRect, 0xFFB5B5F1);
            else
                SDL_FillRect(pBlitsurface, &lRect, 0xFFC5C5C5);
		}
        else if(mHoverSelection == curLinePos )
        {
            lRect.y = ypos+(line*sepHeight)-1;
            SDL_FillRect(pBlitsurface, &lRect, 0xFFE5E5F1);
        }


		trimmedText = *it;

        // If the text is too large to show, show a part of it. (by trimming)
		if(trimmedText.size() > textlimitWidth)
        {
			trimmedText = trimmedText.substr(0, textlimitWidth);
        }

        Font.drawFont(pBlitsurface, trimmedText, xpos, ypos+(line*lRect.h), false);
	}

    mScrollbar.mMaxScrollAmt = mItemList.size()-mScrollbar.lastToShow();

    // Do we need a scrollbar?
    if(mScrollbar.mMaxScrollAmt>0)
    {
        mScrollbar.processRender(displayRect);
        //drawScrollBar(displayRect.SDLRect());
    }

}
