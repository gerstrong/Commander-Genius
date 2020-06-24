#include "BorderedButton.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>



BorderedButton::BorderedButton(const std::string &text,
                               CEvent *ev,
                               const Style style) :
GameButton(text, ev, style)
{}

BorderedButton::BorderedButton(const std::string& text,
                               const GsRect<float> &rect,
                               CEvent *ev,
                               const Style style):
GameButton(text, rect, ev, style)
{}



void BorderedButton::renderWithBorder(const GsRect<float> &rectDispCoordFloat)
{
    GsRect<float> textRect = rectDispCoordFloat;

    textRect.pos.x += 5.0f;
    textRect.dim.x -= 5.0f;

    GameButton::processRender(textRect);

    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();

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
}

void BorderedButton::processRender(const GsRect<float> &rectDispCoordFloat)
{
    renderWithBorder(rectDispCoordFloat);
}

