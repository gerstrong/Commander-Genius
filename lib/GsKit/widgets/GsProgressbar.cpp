
#include <sstream>
#include <iomanip>

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsProgressbar.h"


void GsProgressBar::processLogic()
{
    // Note: So far no logic required. Usually the a reference to mProgress the variable sets the progress
}

void GsProgressBar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    GsRect<float> progressRect = mRect;

    auto pBlitsurface = gVideoDriver.getBlitSurface();

    float progressWidth = 0.0;

    if(mProgress < 0.0 || mProgress > 1000.0)
    {
        progressWidth = displayRect.w;
    }
    else
    {
        progressWidth = (float(mProgress)*displayRect.w)/1000.0f;
    }

    progressRect.w = progressWidth;


    displayRect.transform(RectDispCoordFloat);
    progressRect.transform(RectDispCoordFloat);

    SDL_Rect bgSDLRect = displayRect.SDLRect();
    SDL_Rect progressSDLRect = progressRect.SDLRect();

    Uint32 bgColor = 0;
    Uint32 progressColor = 0;

    auto progressFloat = float(mProgress)/10.0f;

    std::stringstream ss;

    if( mProgress < 0.0 )
    {
        bgColor = SDL_MapRGBA( pBlitsurface->format, 240, 255, 240, 255 );
        progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, 128, 255 );

        ss << "Fetching ...";
    }
    else if( mProgress > 1000.0 )
    {
        bgColor = SDL_MapRGBA( pBlitsurface->format, 240, 255, 240, 255 );
        progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, 128, 255 );

        ss << "Completed!";
    }
    else
    {
        bgColor = SDL_MapRGBA( pBlitsurface->format, 240, 255, 240, 255 );
        progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, 0, 255 );

        ss << "Now at ";
        ss << std::setprecision(1);
        ss << std::fixed;
        ss << progressFloat;
        ss << " %";
    }

    SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);
    SDL_FillRect(pBlitsurface, &progressSDLRect, progressColor);

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);
    Font.drawFontCentered( pBlitsurface, ss.str(), bgSDLRect.x, bgSDLRect.w, bgSDLRect.y, false );
}
