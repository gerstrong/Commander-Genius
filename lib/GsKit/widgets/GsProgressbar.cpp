
#include <sstream>
#include <iomanip>

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsProgressbar.h"


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

    auto progressFloat = float(mProgress)/10.0f;

    std::stringstream ss;

    ss << "Now at ";
    ss << std::setprecision(1);
    ss << std::fixed;
    ss << progressFloat;
    ss << " %";

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);
    Font.drawFontCentered(gVideoDriver.getBlitSurface(), ss.str(), lRect.x, lRect.w, lRect.y, false);
}
