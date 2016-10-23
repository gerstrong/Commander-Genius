#include "BorderedButton.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


namespace galaxy
{


BorderedButton::BorderedButton(const std::string &text, CEvent *ev) :
GalaxyButton(text, ev)
{}




void BorderedButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;

    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    Uint32 newcolor;

    if(!mEnabled)
        newcolor = blitsfc.mapRGB(123, 150, 123);
    else if(mHovered || mPressed)
        newcolor = blitsfc.mapRGB(84, 234, 84);
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
        if(mHovered)
            drawEnabledButton(blitsfc, lRect, true);
        else
            drawEnabledButton(blitsfc, lRect, false);
    }
}


}
