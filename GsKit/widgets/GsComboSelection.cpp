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
GsControlsManager(rect),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() ),
mFeatureText(text)
{

    mpFeatureName =
            addControl(new CGUIText(text,
                                GsRect<float>(0.0f, 0.0f, 0.5f, 1.0f)));

    mpFeatureValue =
            addControl(new GsButton("?",
                                GsRect<float>(0.5f, 0.0f, 0.5f, 1.0f),
                                [&]{this->cycleOption();},
                                style,
                                1.0f, 0.75f, 0.75f));
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
        blitsfc.fill(displayRect, blitsfc.mapRGB(0xBB, 0xBB, 0xFF));
    }


    for(auto &obj : mControlList)
    {
        obj->processRender(displayRect);
    }

	// Transform to the display coordinates
   /* GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    if(!mEnabled)
        return;

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    if( mReleased )
    {
        drawRect( blitsfc, &lRect, 0, 0x00BBBBBB, 0x00CFCFCF );
    }
    else if( mPressed )
    {
        drawRect( blitsfc, &lRect, 0, 0x00BBBBBB, 0x00DFDFDF );
    }
#ifndef DISABLE_HOVER
    else if( mHovered )
    {
        drawRect( blitsfc, &lRect, 0, 0x00BBBBBB, 0x00EFEFEF );
    }
#endif
    else
    {
        drawRect( blitsfc, &lRect, 0, 0x00BBBBBB, 0x00FFFFFF );
    }


    if(!gTTFDriver.isActive())
    {
        // Now lets draw the text of the list control
        GsFontLegacy &Font = gGraphics.getFont(mFontID);

        const int fontHeight = 8;
        //const int textX = lRect.x+24+(mText.size()+2)*fontHeight;
        const int textY = lRect.y+((lRect.h-fontHeight)/2);


        Font.drawFont( blitsfc, mText, lRect.x+24, textY, false );
        //Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, textY, false );
        //const std::string text = (*mOLCurrent);
        //Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*fontHeight, textY, false );
    }
    else
    {
        GsButton::processRender(RectDispCoordFloat);
    }
*/
}
