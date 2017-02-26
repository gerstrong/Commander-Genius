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
             command == IC_POGO || command == IC_FIRE || command == IC_RUN)
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


    const auto halfBorderHeight = (mBorderHeight/2);

    const float textHeight = (pixth+mBorderHeight);

    const float y_innerbound_min = fy /*+ static_cast<float>(textHeight)/bh*/;
	const float y_innerbound_max = y_innerbound_min +
            static_cast<float>( mItemList.size()*textHeight )/bh;

    const float x_innerbound_min = fx + static_cast<float>(textHeight)/bw;


    GsRect<float> rRect(fx, fy, fw, fh);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState();        

    const Vector2D<float> mousePos = pointingState.mPos;

    if( rRect.HasPoint(mousePos) )
    {
        // Let scrollbar do it's work
        mScrollbar.processLogic();

        if( mousePos.y > fy && mousePos.y < y_innerbound_max )
        {
            int newselection = ( ((mousePos.y-fy)*bh- halfBorderHeight )/textHeight) + mScrollbar.scrollPos();

            if( mousePos.x > x_innerbound_min && mousePos.y > y_innerbound_min)
            {
                if(mHoverTriggers)
                {
                    if(mHovered)
                    {
                        mHoverSelection = newselection;
                        mPressedSelection = newselection;
                        mReleasedSelection = newselection;
                    }
                }
                else
                {
                    if(mHovered)
                    {
                        mHoverSelection = newselection;
                    }
                    if(mPressed)
                    {
                        mPressedSelection = newselection;
                    }
                    if(mReleased)
                    {
                        mReleasedSelection = newselection;
                    }
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

void CGUITextSelectionList::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Blit the List surface
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);

    GsRect<Uint16> origRect(displayRect);
    GsRect<Uint16> rect = origRect;

    if(!mEnabled)
    {
        blitsfc.fillRGBA(rect, 0xDF, 0xDF, 0xDF, 0xFF);
    }
    else
    {
        blitsfc.fillRGBA(rect, 0xFF, 0xFF, 0xFF, 0xFF);
    }

	// Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);
    const int pixth = Font.getPixelTextHeight();
    const int pixtw = pixth; // NOTE: We assume here, that the height and width are the same. Invalid to galaxy fonts!

	// Move 16 Pixel so we have space for the cursor/twirl to show the selection
    const auto halfBorderHeight = (mBorderHeight/2);
    const int sepHeight = Font.getPixelTextHeight()+mBorderHeight;
    const int xpos = rect.x+16+1;
    const int ypos = rect.y+10;
    unsigned int textlimitWidth = (rect.w-16)/pixtw;

    mScrollbar.mLastToShow = (rect.h/sepHeight)-1;

    rect.h = pixth+mBorderHeight;

    rect.x += 12;
    rect.w -= 12;

    auto it = mItemList.begin();

    for(int i=0 ; i<mScrollbar.scrollPos() ; it++, i++);

    for ( int line = 0;  it != mItemList.end() && line<mScrollbar.mLastToShow ; it++, line++ )
	{
        // Current line to be rendered
        const int curLinePos = static_cast<int>(line) + mScrollbar.scrollPos();

        if( mPressedSelection == curLinePos )
        {
            rect.y = ypos+(line*rect.h);
            blitsfc.fillRGBA(rect, 0x95, 0xA5, 0xF1, 0xFF);
        }
        else if( mReleasedSelection == curLinePos )
		{
            rect.y = ypos + (line*rect.h);

            if(mSelected)
            {
                blitsfc.fillRGBA(rect, 0xC5, 0xC5, 0xF1, 0xFF);
            }
            else
            {
                blitsfc.fillRGBA(rect, 0xB5, 0xB5, 0xF1, 0xFF);
            }
		}
        else if( mHoverSelection == curLinePos )
        {
            rect.y = ypos+(line*sepHeight);
            blitsfc.fillRGBA(rect, 0xC5, 0xC5, 0xC5, 0xFF);
        }


        std::string trimmedText = *it;

        // If the text is too large to show, show a part of it. (by trimming)
		if(trimmedText.size() > textlimitWidth)
        {
			trimmedText = trimmedText.substr(0, textlimitWidth);
        }

        Font.drawFont(blitsfc, trimmedText, xpos, ypos+(line*rect.h)+halfBorderHeight, false);
	}

    mScrollbar.mMaxScrollAmt = mItemList.size()-mScrollbar.lastToShow();

    // Do we need a scrollbar?
    if(mScrollbar.mMaxScrollAmt>0)
    {
        mScrollbar.processRender(displayRect);
    }

    // Draw a highlighted over everything
    if( mEnabled && mSelected )
    {
        blitsfc.drawFrameRect(origRect, 2, blitsfc.mapRGB(0xB5, 0xB5, 0xF1) );
    }
}
