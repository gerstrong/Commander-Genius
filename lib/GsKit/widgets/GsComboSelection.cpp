/*
 * CGUIComboSelection.cpp
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#include "GsComboSelection.h"
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>

CGUIComboSelection::CGUIComboSelection( const std::string& text,
										     const std::list<std::string>& optionsList ) :
mText(text),
mOptionsList( optionsList ),
mOLCurrent( mOptionsList.begin() )
{

/*	if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		mFontID = 1;
		drawButton = &CGUIComboSelection::drawVorticonStyle;
	}
	else if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
    }
*/
}


const std::string& CGUIComboSelection::getSelection()
{
	return *mOLCurrent;
}


void CGUIComboSelection::cycleOption()
{
	// Cycle through the Optionslist
	mOLCurrent++;
	if( mOLCurrent == mOptionsList.end() )
		mOLCurrent =  mOptionsList.begin();
}



bool CGUIComboSelection::sendEvent(const InputCommands command)
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
	// Getting at this point means, that this option never existed
	mOptionsList.push_back( selectionText );
	mOLCurrent = mOptionsList.end();
	mOLCurrent--;
}





void CGUIComboSelection::setList(const char **strArray, const int numElem)
{
	if(!mOptionsList.empty())
		mOptionsList.clear();


	for( int i=0 ; i<numElem ; i++ )
		mOptionsList.push_back( strArray[i] );

	mOLCurrent = mOptionsList.begin();

}



void CGUIComboSelection::processLogic()
{        
	if(!mEnabled)
		return;

    processPointingState();

    // If combo selecton was pushed and gets released, cycle one option
    if(mReleased)
    {
        cycleOption();
    }
}


void CGUIComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    if(!mEnabled)
        return;

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    if( mReleased )
    {
        drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
    }
    else if( mPressed )
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
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );
}
