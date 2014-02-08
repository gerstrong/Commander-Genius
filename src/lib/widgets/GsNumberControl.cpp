/*
 * CGUINumberControl.cpp
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <lib/base/GsTimer.h>
#include <base/utils/StringUtils.h>

#include "GsNumberControl.h"


int CGUINumberControl::mTwirliconID = 10;

const int SLIDER_WIDTH = 16;



CGUINumberControl::CGUINumberControl(	const std::string& text,
					const int startValue,
					const int endValue,
					const int deltaValue,
					const int value,
					const bool slider ) :
mIncSel(false),
mDecSel(false),
mText(text),
mValue(value),
mSlider(slider),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue)
{
	mFontID = 1;
}


void CGUINumberControl::increment()
{
	setSelection(mValue+mDeltaValue);
}

void CGUINumberControl::decrement()
{
	setSelection(mValue-mDeltaValue);
}


bool CGUINumberControl::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP || command == IC_RIGHT)
	{
		increment();
		return true;
	}
	else if(command == IC_LEFT)
	{
		decrement();
		return true;
	}
	return false;
}



const int CGUINumberControl::getSelection()
{
	return mValue;
}

void CGUINumberControl::setSelection( const int value )
{

	if( mStartValue>value )
		mValue = mStartValue;
	else if( mEndValue<value )
		mValue = mEndValue;
	else
		mValue = value;

    mMustRedraw = true;
}

void CGUINumberControl::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( PointingDevEvent *mouseevent = gInput.m_EventList.occurredEvent<PointingDevEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
            const float xMid = mRect.x+(mRect.w)/2.0f;

			if(mouseevent->Type == PDE_MOVED)
			{
				mDecSel = false;
				mIncSel = false;

                if( MousePos.x < xMid )
					mDecSel = true;
                else if( MousePos.x > xMid )
					mIncSel = true;

				mHovered = true;
				gInput.m_EventList.pop_Event();
			}
			else if(mouseevent->Type == PDE_BUTTONDOWN)
			{
				mPressed = true;
				gInput.m_EventList.pop_Event();
			}
			else if(mouseevent->Type == PDE_BUTTONUP)
			{
				mReleased = true;
				mHovered = true;
				mPressed = false;


                if( MousePos.x < xMid )
				{
					// Cycle through the values
					if( mValue > mStartValue )
						decrement();
				}
                else if( MousePos.x > xMid )
				{
					// Cycle through the values
					if( mValue < mEndValue )
						increment();
				}

                mMustRedraw = true;
				gInput.m_EventList.pop_Event();
			}
		}
		else
		{
			mIncSel = false;
			mDecSel = false;
			mHovered = false;
			mPressed = false;
			mReleased = false;
		}
	}
}


void CGUINumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{

	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
    GsRect<Uint16> lRect(displayRect);

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    if( mReleased )
    {
        blitsfc.drawRect( lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
    }
    else if( mPressed )
    {
        blitsfc.drawRect( lRect, 1, 0x00BBBBBB, 0x00DFDFDF );
    }
    else if( mHovered )
    {
        blitsfc.drawRect( lRect, 1, 0x00BBBBBB, 0x00EFEFEF );
    }
    else
    {
        blitsfc.drawRect( lRect, 1, 0x00BBBBBB, 0x00FFFFFF );
    }

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFontCentered( blitsfc,
                           mText,
                           lRect,
                           false );
}
