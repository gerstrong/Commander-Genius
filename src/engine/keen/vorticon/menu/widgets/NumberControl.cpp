/*
 * CGUINumberControl.cpp
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

/*#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <lib/base/GsTimer.h>
#include <base/utils/StringUtils.h>

#include "NumberControl.h"*/
/*
namespace vorticon
{

int NumberControl::mTwirliconID = 10;

const int SLIDER_WIDTH = 16;



NumberControl::CGUINumberControl(	const std::string& text,
					const int startValue,
					const int endValue,
					const int deltaValue,
					const int value,
					const bool slider ) :
mIncSel(false),
mDecSel(false),
mText(text),
mValue(value),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue),
mSlider(slider)
{
	mFontID = 1;    
	setupButtonSurface();
}


void NumberControl::increment()
{
	setSelection(mValue+mDeltaValue);
}

void NumberControl::decrement()
{
	setSelection(mValue-mDeltaValue);
}


bool NumberControl::sendEvent(const InputCommands command)
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



const int NumberControl::getSelection()
{
	return mValue;
}

void NumberControl::setSelection( const int value )
{

	if( mStartValue>value )
        mValue = mStartValue;
	else if( mEndValue<value )
		mValue = mEndValue;
	else
		mValue = value;

	setupButtonSurface();
}


std::string NumberControl::sliderStr()
{
	int ch;
	ch = (mDecSel) ? 8 : 1;
	std::string slider;
	slider = static_cast<char>(ch);

	const int sVal = (SLIDER_WIDTH-3)*(mValue - mStartValue) / (mEndValue - mStartValue);

	for( int l=0 ; l<sVal ; l++)
		slider += '\04';

	slider += '\05';

	for( int l=0 ; l<(SLIDER_WIDTH-3)-sVal ; l++)
		slider += '\06';

	ch = (mIncSel) ? 9 : 7;
	slider += static_cast<char>(ch);

	return slider;
}





void NumberControl::processLogic()
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
                        increment();een/vorticon/menu/CAudioSettings.cpp.o
				}

				setupButtonSurface();
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


void NumberControl::drawVorticonStyle(SDL_Rect& lRect)
{

	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );

	if(mSlider)
	{
		gGraphics.getFont(2).drawFont( blitsfc, sliderStr(), lRect.x+16+(mText.size()+2)*8, lRect.y, false );
	}
	else
	{
		std::string text = (mDecSel) ? "\025" : " ";
		text += itoa(mValue);
		if(mIncSel)
			text += static_cast<char>(17);
		else
			text += " ";

		Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );
	}

	drawTwirl(lRect);

}


void NumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    drawVorticonStyle(lRect);
}

}
*/
