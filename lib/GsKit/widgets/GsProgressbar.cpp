
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
    GsRect<float> progressRect = mRect;

    auto pBlitsurface = gVideoDriver.getBlitSurface();

    const auto width = (float(mProgress)*displayRect.w)/1000.0f;

    progressRect.w = width;


    displayRect.transform(RectDispCoordFloat);
    progressRect.transform(RectDispCoordFloat);

    SDL_Rect bgSDLRect = displayRect.SDLRect();
    SDL_Rect progressSDLRect = progressRect.SDLRect();

    const auto bgColor = SDL_MapRGBA( pBlitsurface->format, 240, 255, 240, 255 );
    const auto progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, 0, 255 );

    SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);
    SDL_FillRect(pBlitsurface, &progressSDLRect, progressColor);

    auto progressFloat = float(mProgress)/10.0f;

    std::stringstream ss;

    ss << "Now at ";
    ss << std::setprecision(1);
    ss << std::fixed;
    ss << progressFloat;
    ss << " %";

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);
    Font.drawFontCentered( pBlitsurface, ss.str(), bgSDLRect.x, bgSDLRect.w, bgSDLRect.y, false );
}
