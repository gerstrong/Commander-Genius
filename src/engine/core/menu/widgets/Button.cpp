
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GameButton::GameButton(const std::string& text,
                       CEvent *ev,
                       const Style style) :
GsButton(text, ev, style)
{
    if(mStyle == Style::GALAXY)
    {
        mFontID = 1;
        setText(text);
    }
    else if(mStyle == Style::VORTICON)
    {
        mFontID = 1;
    }
}


void GameButton::drawVorticonStyle(SDL_Rect& lRect)
{
    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the button
    if(mEnabled)
    {
        GsFont &Font = gGraphics.getFont(mFontID);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
        drawTwirl(lRect);
    }
    else
    {
        GsFont &Font = gGraphics.getFont(0);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, true );
    }
}




void GameButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();    
    
    if(mStyle == Style::GALAXY)
    {
        GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

        if(!mEnabled)
        {
            mTextDisabledSfc.blitTo(blitsfc, lRect);
        }
        else
        {
            drawEnabledButton(blitsfc, lRect, mHovered);
        }

        drawBlinker(lRect);
    }
    else if(mStyle == Style::VORTICON)
    {
        drawVorticonStyle(lRect);
    }
    else
    {
        drawNoStyle(lRect);
    }
}
