#include "GsScrollbar.h"

#include <base/video/CVideoDriver.h>
#include <base/PointDevice.h>
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

    Vector2D<float> MousePos = pointingState.mPos;

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
    scrollRect.w = 10; // -> This can always stay at font size, the rest must be scaled to the parent control
    GsWeakSurface blitSurface( gVideoDriver.getBlitSurface() );

    SDL_Rect bScUpRect = scrollRect;
    bScUpRect.w  = scrollRect.w;
    bScUpRect.h  = 10;

    SDL_Rect bScDownRect = bScUpRect;

    bScDownRect.y = (scrollRect.y+scrollRect.h) - (bScUpRect.h);

    blitSurface.fillRGB( scrollRect, 0xBF, 0xBF, 0xBF);

    // Now show the slider
    float relPos = float(mScrollPos) / float(mMaxScrollAmt);
    const int posSpace = int(relPos * float(scrollRect.h - (3*10))) + 1;
    SDL_Rect bSliderRect = bScDownRect;
    bSliderRect.x++;
    bSliderRect.y = (bScUpRect.y + bScUpRect.h) + posSpace;
    bSliderRect.w = bScUpRect.w - 2;
    bSliderRect.h = bScUpRect.h - 2;
    blitSurface.fillRGB( bSliderRect, 0x2F, 0x2F, 0x2F);


    // Set the up and down arrows
    GsFont &Font = gGraphics.getFont(mFontID);
    blitSurface.fillRGB( bScUpRect, 0x7F, 0x7F, 0x7F);
    Font.drawFontCentered(blitSurface.getSDLSurface(), "\017", bScUpRect.x, bScUpRect.w, bScUpRect.y, false );
    blitSurface.fillRGB( bScDownRect, 0x7F, 0x7F, 0x7F);
    Font.drawFontCentered(blitSurface.getSDLSurface(), "\023", bScDownRect.x, bScDownRect.w, bScDownRect.y, false );

    mArrowHeight = 10.0f/float(lRect.h);
    mSliderHeight = 8.0f/float(lRect.h);
}

void GsScrollbar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);

    SDL_Rect lRect = displayRect.SDLRect();

    drawScrollBar(lRect);
}

bool GsScrollbar::sendEvent(const InputCommands command)
{
    return false;
}

