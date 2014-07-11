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
#include <base/utils/StringUtils.h>

#include "NumberControl.h"

namespace vorticon
{


const int SLIDER_WIDTH = 16;


NumberControl::NumberControl(const std::string& text,
					const int startValue,
					const int endValue,
					const int deltaValue,
					const int value,
                    const bool slider ) :
CGUINumberControl(text,
    startValue,
    endValue,
    deltaValue,
    value, 1,
    slider )
{
	mFontID = 1;    
}



void NumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
    Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );

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

        Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );
    }

    drawTwirl(lRect);

}

}

