/*
 * CGUIComboSelection.cpp
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#include "GsComboSelection.h"
#include <graphics/GsGraphics.h>
#include <base/GsLogging.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <base/GsTTFSystem.h>

#include <algorithm>

CGUIComboSelection::
CGUIComboSelection( const std::string& text,
                    const GsRect<float> &rect,
                    const std::list<std::string>& optionsList ) :
GsWidgetsManager(rect),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() ),
mFeatureText(text)
{
    spawnSubWidgets();
}

void CGUIComboSelection::updateFeatureDim()
{
    // In some cases the value letters are very small.
    // In such case create more room for the name
    int maxOptionLen = 0;
    float featNameWidth = 0.5f;

    for( auto &option : mOptionsList)
    {
        maxOptionLen = std::max(int(option.length()), maxOptionLen);
    }

    if(maxOptionLen < 4)
        featNameWidth = 0.25f;

    const float featValWidth = 1.0f-featNameWidth;

    mpFeatureName->setRect(GsRect<float>(0.0f, 0.0f, featValWidth, 1.0f));
    mpFeatureValue->setRect(GsRect<float>(featValWidth, 0.0f, featNameWidth, 1.0f));
}


void CGUIComboSelection::spawnSubWidgets()
{
    mpFeatureName =
            add(new GsText(mFeatureText,
                                GsRect<float>(0.0f, 0.0f, 0.5f, 1.0f)));

    mpFeatureValue =
            add(new GsButton("?",
                         GsRect<float>(0.5f, 0.0f, 0.5f, 1.0f),
                         [&]{this->cycleOption();}));

    updateFeatureDim();
}


void CGUIComboSelection::cycleOption()
{
	// Cycle through the Optionslist
	mOLCurrent++;
    if( mOLCurrent == mOptionsList.end() )
    {
		mOLCurrent =  mOptionsList.begin();
    }

    mpFeatureValue->setText(*mOLCurrent);
}



bool CGUIComboSelection::sendEvent(const InpCmd command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		cycleOption();
		return true;
	}
	return false;
}


void CGUIComboSelection::setSelection( const std::string& selectionText )
{
    auto &&optionIt = mOptionsList.cbegin();

    for ( ; optionIt != mOptionsList.cend() ; optionIt++ )
    {
        if(*optionIt == selectionText)
        {
            mOLCurrent = optionIt;
            mpFeatureValue->setText(*mOLCurrent);
            break;
        }
    }

    if( optionIt == mOptionsList.cend())
    {
        gLogging << "Warning: Option " <<  selectionText
                 << " does not exist for this selection.";
    }
}


void CGUIComboSelection::setList(const std::set<std::string> &strSet)
{
    mOptionsList.clear();

    for( const auto &str : strSet)
    {
        mOptionsList.push_back( str );
    }

	mOLCurrent = mOptionsList.begin();
    mpFeatureValue->setText(*mOLCurrent);

    updateFeatureDim();
}


void CGUIComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    if(mHighlightBg)
    {
        blitsfc.fill(displayRect, mFillColor.toUint32(blitsfc));
    }

    auto &wList = getWidgetList();
    for(auto &obj : wList)
    {
        obj->processRender(displayRect);
    }
}

void CGUIComboSelection::processLogic()
{
    if(mReleased)
    {
        // That makes the button trigger exaclty once
        mpFeatureValue->setReleased(true);
    }

    mFillColor.ponder(0.075f);

    if(mHighlightBg)
    {
        if(!mEnabled)
        {
            mFillColor.setTargetColor(mDisabledColor);
        }
        else
        {
            if(mSelected)
            {
                mFillColor.setTargetColor(mSelectedColor);
            }
            else
            {
                GsColor selectFillColor = mEnabledColor;

                if(mHovered)
                    selectFillColor.converge(GsColor(255,255,255));
                if(mPressed)
                    selectFillColor.converge(mSelectedColor);
                else if(mReleased)
                    selectFillColor = mSelectedColor;

                mFillColor.setTargetColor(selectFillColor);
            }
        }
    }

    GsWidgetsManager::processLogic();
}

void CGUIComboSelection::setTextColor(const GsColor &color)
{
    mpFeatureName->setTextColor(color);
    mpFeatureValue->setTextColor(color);
}

void CGUIComboSelection::enableButtonBorders(const bool value)
{
    mpFeatureValue->enableBorder(value);
}

void CGUIComboSelection::enableWidgetsCenteringH(const bool value)
{
    mpFeatureName->enableCenteringH(value);
    mpFeatureValue->enableCenteringH(value);
}
