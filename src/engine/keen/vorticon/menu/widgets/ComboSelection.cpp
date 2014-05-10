/*
 * CGUIComboSelection.cpp
 *
 *  Created on: 26.02.2012
 *      Author: gerstrong
 */

#include "ComboSelection.h"
#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>


namespace vorticon
{


ComboSelection::ComboSelection( const std::string &text,
                                const std::list<std::string> &optionsList ) :
CGUIComboSelection(text, optionsList)
{
    mFontID = 1;
}


void ComboSelection::processRender(const GsRect<float> &RectDispCoordFloat)
{    
    if(!mEnabled)
        return;

	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
    Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );
    const std::string text = (*mOLCurrent);
    Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );

    drawTwirl(lRect);
}

}

