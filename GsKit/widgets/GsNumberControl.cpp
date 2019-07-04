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


const int SLIDER_WIDTH = 16;

CGUINumberControl::CGUINumberControl(const std::string& text,
                                     const GsRect<float> &rect,
                                      const int startValue,
                                      const int endValue,
                                      const int deltaValue,
                                      const int value,
                                      const bool slider) :
GsWidgetsManager(rect),
mValue(value),
mSlider(slider),
mStartValue(startValue),
mEndValue(endValue),
mDeltaValue(deltaValue),
mHoverBgColor(0xBB, 0xBB, 0xFF),
mFeatureText(text)
{    
    spawnSubWidgets();
}


void CGUINumberControl::spawnSubWidgets()
{
    mpCtrlName =
            add(new CGUIText(mFeatureText,
                                GsRect<float>(0.0f, 0.0f, 0.5f, 1.0f)));

    mpLeftButton =
            add(new GsButton("<",
                                GsRect<float>(0.5f, 0.0f, 0.1f, 1.0f),
                                [&]{this->decrement();}));

    mpCtrlValue =
            add(new CGUIText("?",
                                GsRect<float>(0.6f, 0.0f, 0.3f, 1.0f)));


    mpRightButton =
            add(new GsButton(">",
                                GsRect<float>(0.9f, 0.0f, 0.1f, 1.0f),
                                [&]{this->increment();}));
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

    if(mHovered && mHighlightBg)
    {
        blitsfc.fill(displayRect, blitsfc.mapColorAlpha(mHoverBgColor));
    }

    auto &widgetsList = getWidgetList();

    for( auto &ctrl : widgetsList)
    {
        ctrl->processRender(displayRect);
    }
}

void CGUINumberControl::setTextColor(const GsColor &color)
{
    mpCtrlName->setTextColor(color);
    mpLeftButton->setTextColor(color);
    mpCtrlValue->setTextColor(color);
    mpRightButton->setTextColor(color);
}

void CGUINumberControl::enableButtonBorders(const bool value)
{
    mpLeftButton->enableBorder(value);
    mpRightButton->enableBorder(value);
}

void CGUINumberControl::enableWidgetsCenteringH(const bool value)
{
    mpCtrlName->enableCenteringH(value);
    mpLeftButton->enableCenteringH(value);
    mpCtrlValue->enableCenteringH(value);
    mpRightButton->enableCenteringH(value);
}


