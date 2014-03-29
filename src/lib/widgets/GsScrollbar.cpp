#include "GsScrollbar.h"

#include <base/video/CVideoDriver.h>
#include <lib/base/PointDevice.h>
#include <graphics/GsGraphics.h>


void GsScrollbar::processLogic()
{
    // Here we check if the mouse-cursor/Touch entry clicked on something!!

    const float bw = gVideoDriver.getGameResolution().w;
    const float bh = gVideoDriver.getGameResolution().h;

    const float fx = mRect.x;
    const float fw = mRect.w;
    const float fy = mRect.y;
    const float fh = mRect.h;

    const float y_innerbound_min = fy;
    const float y_innerbound_max = fy+fh;

    //const float x_innerbound_min = fx + static_cast<float>(TEXT_HEIGHT)/bw;

    GsRect<float> rRect(fx, fy, fw, fh);
    GsRect<float> parRect(mpParent->mRect);

    rRect.transform(parRect);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState(rRect);

    CVec MousePos = pointingState.mPos;

    if( rRect.HasPoint(MousePos) )
    {
        if( MousePos.y > fy && MousePos.y < y_innerbound_max )
        {
            if(mReleased) // If clicked on scroll bar
            {
                const float midPart_y = (fy+y_innerbound_max)/2.0;

                if(MousePos.y < midPart_y) // Go up!
                {
                    scrollUp();
                }
                else if(MousePos.y > midPart_y) // Go down!
                {
                    scrollDown();
                }
            }
        }
    }
}

void GsScrollbar::drawScrollBar(const SDL_Rect &lRect)
{
    SDL_Rect scrollRect = lRect;
    SDL_Surface *Blitsurface = gVideoDriver.getBlitSurface();

    SDL_Rect bScUpRect = scrollRect;
    bScUpRect.w  = 10;
    bScUpRect.h  = 10;

    SDL_Rect bScDownRect = bScUpRect;

    bScDownRect.y = (scrollRect.y+scrollRect.h) - (bScUpRect.h);

    SDL_FillRect(Blitsurface, &scrollRect, 0xFFBFBFBF);

    // Now show the slider
    float relPos = float(scrollPos()) / float(mMaxScrollAmt);
    const int posSpace = int(relPos * float(scrollRect.h - (3*10))) + 1;
    SDL_Rect bSliderRect = bScDownRect;
    bSliderRect.x++;
    bSliderRect.y = (bScUpRect.y + bScUpRect.h) + posSpace;
    bSliderRect.w = 8;
    bSliderRect.h = 8;
    SDL_FillRect(Blitsurface, &bSliderRect, 0xFF2F2F2F);

    const Uint32 scButtonColor = 0xFF7F7F7F;

    // Set the up and down arrows
    GsFont &Font = gGraphics.getFont(mFontID);
    SDL_FillRect(Blitsurface, &bScUpRect,   scButtonColor);
    Font.drawFont(Blitsurface, "\017", bScUpRect.x+1, bScUpRect.y+2, false);
    SDL_FillRect(Blitsurface, &bScDownRect, scButtonColor);
    Font.drawFont(Blitsurface, "\023", bScDownRect.x+1, bScDownRect.y, false);
}

void GsScrollbar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Blit the List surface
    GsWeakSurface blitsurface(gVideoDriver.getBlitSurface());

    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);

    SDL_Rect lRect = displayRect.SDLRect();
    blitsurface.fillRGB(lRect, 0, 128, 0);

    drawScrollBar(lRect);
}

bool GsScrollbar::sendEvent(const InputCommands command)
{
    return false;
}

