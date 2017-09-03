
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GalaxyButton::GalaxyButton(const std::string& text,
                           CEvent *ev) :
GsButton(text, ev)
{
    mFontID = 1;

    setupButtonSurface("  " + mText);
}


void GalaxyButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

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
