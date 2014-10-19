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
{}


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

    const int fontHeight = 8;
    //const int textX = lRect.x+24+(mText.size()+2)*fontHeight;
    const int textY = lRect.y+((lRect.h-fontHeight)/2);


    Font.drawFont( blitsfc, mText, lRect.x+24, textY, false );
    Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, textY, false );

    const std::string text = (*mOLCurrent);
    Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*fontHeight, textY, false );

}
