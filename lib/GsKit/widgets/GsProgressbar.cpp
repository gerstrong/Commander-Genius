
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsProgressbar.h"

GsProgressBar::GsProgressBar()
{

}


void GsProgressBar::processLogic()
{
    // TODO: Logic of the progress bar
}

void GsProgressBar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // TODO: Rendering of the progress bar
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);
    Font.drawFontCentered(gVideoDriver.getBlitSurface(), "Doing some Progress", lRect.x, lRect.w, lRect.y, false);
}
