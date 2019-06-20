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
#include <base/GsTTFSystem.h>

#include "GsNumberControl.h"


int CGUINumberControl::mTwirliconID = 10;

const int SLIDER_WIDTH = 16;
//const int BLEND_SPEED = 16;


CGUINumberControl::CGUINumberControl(const std::string& text,
                                     const GsRect<float> &rect,
                                      const int startValue,
                                      const int endValue,
                                      const int deltaValue,
                                      const int value,
                                      const int fontid,
                                      const bool slider) :
GsWidgetsManager(rect),
mValue(value),
mSlider(slider),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue)
{
    //mFontID = fontid;

    mpLeftButton =
            addWidget(new GsButton("<",
                                GsRect<float>(0.0f, 0.0f, 0.1f, 1.0f),
                                [&]{this->decrement();},
                                fontid));

    mpCtrlName =
            addWidget(new CGUIText(text,
                                GsRect<float>(0.1f, 0.0f, 0.4f, 1.0f),
                                    fontid));
    mpCtrlValue =
            addWidget(new CGUIText("?",
                                GsRect<float>(0.5f, 0.0f, 0.4f, 1.0f),
                                    fontid));


    mpRightButton =
            addWidget(new GsButton(">",
                                GsRect<float>(0.9f, 0.0f, 0.1f, 1.0f),
                                [&]{this->increment();},
                                fontid));


}


void CGUINumberControl::increment()
{
	setSelection(mValue+mDeltaValue);
}

void CGUINumberControl::decrement()
{
	setSelection(mValue-mDeltaValue);
}


bool CGUINumberControl::sendEvent(const InputCommand command)
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



int CGUINumberControl::getSelection()
{
	return mValue;
}

void CGUINumberControl::setSelection( const int value )
{
    mpLeftButton->enable(true);
    mpRightButton->enable(true);

    if( mStartValue>value )
		mValue = mStartValue;
	else if( mEndValue<value )
		mValue = mEndValue;
	else
		mValue = value;

    mpLeftButton->enable( mValue > mStartValue ? true : false  );
    mpRightButton->enable( mValue < mEndValue ? true : false  );

    mMustRedraw = true;
    mpCtrlValue->setText(itoa(mValue));
}

void CGUINumberControl::processLogic()
{
    GsWidgetsManager::processLogic();
    /*

    GsPointingState &pointingState = gPointDevice.mPointingState;

    const bool hasPoint = mRect.HasPoint(pointingState.mPos);
    const bool bDown = (pointingState.mActionButton>0);

    const float xMid = mRect.pos.x+(mRect.dim.x/2.0f);

    mReleased = false;

    mDecSel = false;
    mIncSel = false;

    GsVec2D<float> mousePos = pointingState.mPos;

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

#ifdef DISABLE_HOVER
    if(bDown)
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
        if(mSelected)
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
#else
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
#endif
*/
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


void CGUINumberControl::processRender(const GsRect<float> &rectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();
    displayRect.transform(rectDispCoordFloat);

    if(mHovered)
    {
        blitsfc.fill(displayRect, blitsfc.mapRGB(0xBB, 0xBB, 0xFF));
    }


    for(auto &obj : mWidgetList)
    {
        obj->processRender(displayRect);
    }
    /*
    if(lRect.h == 0 || lRect.w == 0)
        return;

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int lComp = 0xFF;

    // Set as default a grey border
    Uint32 borderColor = blitsfc.mapRGBA( 0xBB, 0xBB, 0xBB, 0xFF);

    if(mEnabled)
    {
        auto lightRatio = mLightRatio;

        if( mPressed || mSelected )
        {
            // Try to highlight the border color a bit more by determing which one dominates the most
            auto red   = Uint8(mRed*127.0f);
            auto green = Uint8(mGreen*127.0f);
            auto blue  = Uint8(mBlue*127.0f);

            if(red > green && red > blue)
                red <<= 1;
            else if(green > red  && green > blue)
                green <<= 1;
            else if(blue > green && blue > red )
                blue <<= 1;

            if(mPressed)
            {
                red <<= 1;
                blue >>= 1;
                lightRatio <<= 1;
            }

            // If want to highlight the button set the color
            borderColor = blitsfc.mapRGBA( red, green, blue, 0xFF);

            lComp = 0xFF - (lightRatio*(0xFF-0xCF)/255);
        }
        else
        {
            lComp = 0xFF - (lightRatio*(0xFF-0xDF)/255);
        }
    }


    auto lcompf = float(lComp);

    auto redC   = Uint8(lcompf*mRed);
    auto greenC = Uint8(lcompf*mGreen);
    auto blueC  = Uint8(lcompf*mBlue);

    const auto fillColor = blitsfc.mapRGBA( redC, greenC, blueC, 0xFF);

    GsRect<Uint16> rect(lRect);

    blitsfc.drawRect( rect, 2, borderColor, fillColor );

    if(!gTTFDriver.isActive())
    {
        // Now lets draw the text of the list control
        auto &Font = gGraphics.getFont(mFontID);

        if(mEnabled) // If the button is enabled use the normal text, otherwise the highlighted color
        {
            Font.drawFontCentered( blitsfc.getSDLSurface(), mText,
                                   lRect.x, lRect.w, lRect.y, lRect.h, false );
        }
        else
        {
            Font.drawFontCentered( blitsfc.getSDLSurface(), mText,
                                   lRect.x, lRect.w, lRect.y, lRect.h, true );
        }
    }
    */
}
