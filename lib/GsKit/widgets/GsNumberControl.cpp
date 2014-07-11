/*
 * CGUINumberControl.cpp
 *
 *  Created on: 06.03.2012
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <base/PointDevice.h>
#include <base/utils/StringUtils.h>


#include "GsNumberControl.h"


int CGUINumberControl::mTwirliconID = 10;

const int SLIDER_WIDTH = 16;
const int BLEND_SPEED = 16;


CGUINumberControl::CGUINumberControl(	const std::string& text,
					const int startValue,
					const int endValue,
					const int deltaValue,
					const int value,
                    const int fontid,
					const bool slider ) :
mIncSel(false),
mDecSel(false),
mText(text),
mValue(value),
mSlider(slider),
mLightRatio(128),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue)
{
    mFontID = fontid;
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
    GsPointingState &pointingState = gPointDevice.mPointingState;

    const bool hasPoint = mRect.HasPoint(pointingState.mPos);
    const bool bDown = (pointingState.mActionButton>0);

    const float xMid = mRect.x+(mRect.w/2.0f);

    mReleased = false;

    mDecSel = false;
    mIncSel = false;

    Vector2D<float> mousePos = pointingState.mPos;

    if( mousePos.x < xMid )
        mDecSel = true;
    else if( mousePos.x > xMid )
        mIncSel = true;


    if(!bDown && mPressed)
    {
        mPressed = false;

        if(hasPoint)
        {
            mReleased = true;
        }
    }

    if(!bDown || mPressed)
    {
        mHovered = hasPoint;
    }

    if(mHovered && bDown)
    {
        mPressed = true;

        if( mDecSel )
        {
            // Cycle through the values -> go one value down
            if( mValue > mStartValue )
                decrement();
        }
        else if( mIncSel )
        {
            // Cycle through the values -> go one value up
            if( mValue < mEndValue )
                increment();
        }

        mMustRedraw = true;
    }


    if(mEnabled)
    {
        // For some nice special effects
        if(mHovered || mSelected)
        {
            if(mLightRatio+BLEND_SPEED < 255)
               mLightRatio += BLEND_SPEED;
            else
               mLightRatio = 255;
        }
        else // Button is not hovered
        {
            if(mLightRatio-BLEND_SPEED > 0)
               mLightRatio -= BLEND_SPEED;
            else
               mLightRatio = 0;
        }
    }

}


std::string CGUINumberControl::sliderStr()
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


void CGUINumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
    GsRect<Uint16> lRect(displayRect);

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int lComp;

    if( mPressed || mSelected )
        lComp = 0xFF - (mLightRatio*(0xFF-0xCF)/255);
    else
        lComp = 0xFF - (mLightRatio*(0xFF-0xDF)/255);

    const Uint32 fillColor = blitsfc.mapRGBA( lComp, lComp, lComp, 0xFF);

    GsRect<Uint16> rect(lRect);

    blitsfc.drawRect( rect, 1, 0xFFBBBBBB, fillColor );


    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    const int fontHeight = 8;
    const int textX = lRect.x+24+(mText.size()+2)*8;
    const int textY = lRect.y+((lRect.h-fontHeight)/2);


    Font.drawFont( blitsfc, mText, lRect.x+24, textY, false );
    Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, textY, false );

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

        Font.drawFont( blitsfc, text, textX, textY, false );
    }

}
