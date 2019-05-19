#include "GsBanner.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


const int TIME_UNTIL_CHANGE = 180;
const int TIME_TRANSITION = 30;


CGUIBanner::CGUIBanner(const std::string& text, const GsRect<float> &rect) :
CGUIText(text, rect)
{
    curTextIt = mTextVec.begin();
}


void CGUIBanner::setText(const std::string& text)
{
    CGUIText::setText(text);
    curTextIt = mTextVec.begin();
}

void CGUIBanner::processLogic()
{        
    if(mTransition)
    {
        if(timer >= TIME_TRANSITION)
        {
            timer = 0;
            alpha = 0;
            mTransition = !mTransition;

            return;
        }
    }
    else
    {
        if(timer >= TIME_UNTIL_CHANGE)
        {
            timer = 0;

            prevTextIt = curTextIt;

            curTextIt++;

            if( curTextIt == mTextVec.end() )
                curTextIt = mTextVec.begin();

            alpha = 0;
            mTransition = !mTransition;

            return;
        }
    }
    
    alpha = uint8_t((255*timer)/TIME_TRANSITION);
    
    timer++;
    
}

void CGUIBanner::processRender(const GsRect<float> &RectDispCoordFloat)
{ 

#if defined(USE_SDL_TTF)

    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);

    //updateFontState(displayRect);
    CGUIText::processRender(RectDispCoordFloat);

    /*
    auto &blit = gVideoDriver.gameSfc();

    if(mTextSfc)
    {
        const auto textW = mTextSfc.width();
        const auto textH = mTextSfc.height();
        GsVec2D<int> textSfcDim(textW, textH);

        GsRect<float> blitPos = displayRect;
        blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;

        mTextSfc.blitTo(blit, blitPos.SDLRect());
    }
*/

#else
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    // Now lets draw the text of the list control
    GsFontLegacy &Font = gGraphics.getFont(mFontID);

    if(mTransition)
    {
        Font.drawFontCenteredAlpha(gVideoDriver.getBlitSurface(), *prevTextIt, lRect.x, lRect.w, lRect.y, 255-alpha);
        Font.drawFontCenteredAlpha(gVideoDriver.getBlitSurface(), *curTextIt, lRect.x, lRect.w, lRect.y, alpha);
    }
    else
    {
        Font.drawFontCentered(gVideoDriver.getBlitSurface(), *curTextIt, lRect.x, lRect.w, lRect.y, false);
    }
#endif
}
