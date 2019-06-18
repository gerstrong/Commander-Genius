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


CGUIComboSelection::CGUIComboSelection( const std::string& text,
                                        const GsRect<float> &rect,
                                        const std::list<std::string>& optionsList,
                                        const GsControl::Style style ) :
GsControlsManager(rect, style),
mHoverBgColor(0xBB, 0xBB, 0xFF),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() ),
mFeatureText(text)
{

    mpFeatureName =
            addControl(new CGUIText(text,
                                GsRect<float>(0.0f, 0.0f, 0.5f, 1.0f),
                                    style));    

    mpFeatureValue =
            addControl(new GsButton("?",
                                GsRect<float>(0.5f, 0.0f, 0.5f, 1.0f),
                                [&]{this->cycleOption();},
                                style));
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



bool CGUIComboSelection::sendEvent(const InputCommand command)
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
}


void CGUIComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    auto displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);

    if(mHovered)
    {
        blitsfc.fill(displayRect, blitsfc.mapColorAlpha(mHoverBgColor));
    }


    for(auto &obj : mControlList)
    {
        obj->processRender(displayRect);
    }

}

void CGUIComboSelection::processLogic()
{
    for(auto &obj : mControlList)
    {
        obj->setHovered(mHovered);
        obj->select(mSelected);
        obj->enable(mEnabled);
        obj->setPressed(mPressed);
        obj->setReleased(mReleased);
    }

    GsControlsManager::processLogic();
}

void CGUIComboSelection::setTextColor(const GsColor &color)
{
    mpFeatureName->setTextColor(color);
    mpFeatureValue->setTextColor(color);
}

void CGUIComboSelection::setTextColorHovered(const GsColor &color)
{
    mpFeatureName->setTextColorHovered(color);
    mpFeatureValue->setTextColorHovered(color);
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
