#include "BorderedButton.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>



BorderedButton::BorderedButton(const std::string &text,
                               CEvent *ev,
                               const GsControl::Style style) :
GameButton(text, ev, style)
{}

BorderedButton::BorderedButton(const std::string& text,
                               const GsRect<float> &rect,
                               CEvent *ev,
                               const GsControl::Style style):
GameButton(text, rect, ev, style)
{}





void BorderedButton::processRender(const GsRect<float> &rectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;

    displayRect.transform(rectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    Uint32 newcolor;

    if(!mEnabled)
        newcolor = blitsfc.mapRGB(123, 150, 123);
#ifndef DISABLE_HOVER
    else if(mHovered || mPressed)
        newcolor = blitsfc.mapRGB(84, 234, 84);
#else
    else if(mPressed)
        newcolor = blitsfc.mapRGB(84, 234, 84);
#endif
    else
        newcolor = blitsfc.mapRGB(38, 134, 38);

    blitsfc.drawFrameRect( lRect, 1, newcolor);

    lRect.y +=2;
    lRect.x +=2;

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(blitsfc, lRect);
    }
    else
    {
#ifndef DISABLE_HOVER
        if(mHovered)
        {
            drawEnabledButton(blitsfc, lRect, true);
        }
        else
#endif
        {
            drawEnabledButton(blitsfc, lRect, false);
        }
    }
}

