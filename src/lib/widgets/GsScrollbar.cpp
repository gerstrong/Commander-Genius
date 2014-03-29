#include "GsScrollbar.h"

#include <base/video/CVideoDriver.h>
#include <lib/base/PointDevice.h>
#include <graphics/GsGraphics.h>


void GsScrollbar::processLogic()
{
    // Here we check if the mouse-cursor/Touch entry clicked on something!!
    const float fx = mRect.x;
    const float fw = mRect.w;
    const float fy = mRect.y;
    const float fh = mRect.h;

    const float y_innerbound_max = fy+fh;

    //const float x_innerbound_min = fx + static_cast<float>(TEXT_HEIGHT)/bw;

    GsRect<float> rRect(fx, fy, fw, fh);
    GsRect<float> parRect(mpParent->mRect);

    rRect.transform(parRect);

    GsPointingState &pointingState = gPointDevice.mPointingState;

    processPointingState(rRect);

    CVec MousePos = pointingState.mPos;

    const bool scrollAction = mReleased || mPressed;

    if( rRect.HasPoint(MousePos) )
    {
        if( MousePos.y > fy && MousePos.y < y_innerbound_max )
        {
            if(scrollAction) // If clicked or pressed on scroll bar
            {
                float relPos = float(mScrollPos) / float(mMaxScrollAmt);

                const float scrollAreaY1 = rRect.y + mArrowHeight + mSliderHeight*0.5f;
                const float scrollAreaY2 = rRect.y + rRect.h - mArrowHeight - mSliderHeight*0.5f;
                const float scrollHeight = scrollAreaY2-scrollAreaY1;

                const float midPart_y = scrollAreaY1+(scrollHeight*relPos);

                if(mPressed)
                {
                   if(MousePos.y < scrollAreaY1 || MousePos.y > scrollAreaY2)
                       return;
                }

                // The tolerance will remove the jitter effect in scrolling
                const float tolerance = scrollHeight/float(mMaxScrollAmt);

                if(MousePos.y < midPart_y-tolerance) // Go up!
                {
                    scrollUp();
                }
                else if(MousePos.y > midPart_y+tolerance) // Go down!
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
    bScUpRect.w  = scrollRect.w;
    bScUpRect.h  = 10;

    SDL_Rect bScDownRect = bScUpRect;

    bScDownRect.y = (scrollRect.y+scrollRect.h) - (bScUpRect.h);

    SDL_FillRect(Blitsurface, &scrollRect, 0xFFBFBFBF);

    // Now show the slider
    float relPos = float(mScrollPos) / float(mMaxScrollAmt);
    const int posSpace = int(relPos * float(scrollRect.h - (3*10))) + 1;
    SDL_Rect bSliderRect = bScDownRect;
    bSliderRect.x++;
    bSliderRect.y = (bScUpRect.y + bScUpRect.h) + posSpace;
    bSliderRect.w = bScUpRect.w - 2;
    bSliderRect.h = bScUpRect.h - 2;
    SDL_FillRect(Blitsurface, &bSliderRect, 0xFF2F2F2F);

    const Uint32 scButtonColor = 0xFF7F7F7F;

    // Set the up and down arrows
    GsFont &Font = gGraphics.getFont(mFontID);
    SDL_FillRect(Blitsurface, &bScUpRect,   scButtonColor);
    Font.drawFontCentered(Blitsurface,"\017", bScUpRect.x, bScUpRect.w, bScUpRect.y, false );
    //Font.drawFont(Blitsurface, "\017", bScUpRect.x+Font.w, bScUpRect.y+2, false);
    SDL_FillRect(Blitsurface, &bScDownRect, scButtonColor);
    //Font.drawFont(Blitsurface, "\023", bScDownRect.x+1, bScDownRect.y, false);
    Font.drawFontCentered(Blitsurface,"\023", bScDownRect.x, bScDownRect.w, bScDownRect.y, false );

    mArrowHeight = 10.0f/float(lRect.h);
    mSliderHeight = 8.0f/float(lRect.h);
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

