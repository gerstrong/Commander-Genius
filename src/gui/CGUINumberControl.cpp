/*
 * CGUINumberControl.cpp
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#include "CGUINumberControl.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "common/CBehaviorEngine.h"
#include "core/CGameMode.h"
#include "sdl/CTimer.h"

int CGUINumberControl::mTwirliconID = 10;


CGUINumberControl::CGUINumberControl(	const std::string& text,
										const int startValue,
										const int endValue,
										const int deltaValue,
										const int value,
										const Style	style ) :
mText(text),
mHovered(false),
mButtonDown(false),
mButtonUp(false),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue),
mValue(value),
drawButton(&CGUINumberControl::drawNoStyle)
{
	if(style == VORTICON)
		drawButton = &CGUINumberControl::drawVorticonStyle;
}


void CGUINumberControl::increment()
{
	setSelection(mValue+mDeltaValue);
}


const int CGUINumberControl::getSelection()
{
	return mValue;
}

void CGUINumberControl::setSelection( int value )
{

	if( mStartValue<value )
		mValue = mStartValue;
	else if( mEndValue<value )
		mValue = mEndValue;
	else
		mValue = value;

}




void CGUINumberControl::processLogic()
{
	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( MouseMoveEvent *mouseevent = g_pInput->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
			if(mouseevent->Type == MOUSEEVENT_MOVED)
			{
				mHovered = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
			{
				mButtonDown = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONUP)
			{
				mButtonUp = true;
				mHovered = true;
				mButtonDown = false;

				// Cycle through the Optionslist
				increment();

				if( mValue > mEndValue )
					mValue = mStartValue;

				g_pInput->m_EventList.pop_Event();
			}
		}
		else
		{
			mHovered = false;
			mButtonDown = false;
			mButtonUp = false;
		}
	}
}


void CGUINumberControl::drawVorticonStyle(SDL_Rect& lRect)
{

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );
	const std::string text = itoa(mValue);
	Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );


	if( g_pTimer->HasTimeElapsed(100) )
	{
		mTwirliconID++;

		if(mTwirliconID == 15)
			mTwirliconID = 9;
	}

	if( mButtonDown )
	{
		Font.drawCharacter( blitsfc, mTwirliconID, lRect.x+12, lRect.y );
	}
	else if( mHovered )
	{
		Font.drawCharacter( blitsfc, mTwirliconID, lRect.x+8, lRect.y );
	}

}


void CGUINumberControl::drawNoStyle(SDL_Rect& lRect)
{

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if( mButtonUp )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
	}
	else if( mButtonDown )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00DFDFDF );
	}
	else if( mHovered )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00EFEFEF );
	}
	else
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00FFFFFF );
	}

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(0);

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );

}


void CGUINumberControl::processRender(const CRect<float> &RectDispCoordFloat)
{

	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);

}
