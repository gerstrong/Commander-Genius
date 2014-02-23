#include "BorderedButton.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


namespace galaxy
{


BorderedButton::BorderedButton(const std::string &text, CEvent *ev) :
GalaxyButton(text, ev)
{

}


void BorderedButton::drawGalaxyBorderedStyle(SDL_Rect& lRect)
{
    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

    const Uint32 oldcolor = Font.getFGColor();

    Uint32 newcolor;

    if(!mEnabled)
        newcolor = SDL_MapRGB( format, 123, 150, 123);
    else if(mHovered || mPressed)
        newcolor = SDL_MapRGB( format, 84, 234, 84);
    else
        newcolor = SDL_MapRGB( format, 38, 134, 38);

    Font.setupColor( newcolor );

    drawEmptyRect( blitsfc, &lRect, newcolor);

    Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y+2, false );

    Font.setupColor( oldcolor );
}


void BorderedButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    drawGalaxyBorderedStyle(lRect);
}


}
