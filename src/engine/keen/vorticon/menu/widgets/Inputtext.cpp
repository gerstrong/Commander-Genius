
#include "Inputtext.h"

#include <base/video/CVideoDriver.h>
#include <graphics/GsFont.h>
#include <graphics/GsGraphics.h>


namespace vorticon
{

void InputText::processRender(const GsRect<float> &RectDispCoordFloat)
{
    if(!mEnabled)
        return;

    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.drawFont( blitsfc, getInputString(), lRect.x+24, lRect.y, false );

    drawTwirl(lRect);
}

}

