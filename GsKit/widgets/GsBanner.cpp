#include "GsBanner.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


const int TIME_UNTIL_CHANGE = 180;
const int TIME_TRANSITION = 30;


CGUIBanner::CGUIBanner(const std::string& text, const GsRect<float> &rect) :
CGUIText(text, rect)
{
}


void CGUIBanner::setText(const std::string& text)
{
    CGUIText::setText(text);
    mCurLineIdx = 0;
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

            mPreLineIdx = mCurLineIdx;

            mCurLineIdx++;

            if( mCurLineIdx >= mTextVec.size() )
                mCurLineIdx = 0;

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

    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);

    updateTTFTextSfc(displayRect);

    auto &blit = gVideoDriver.gameSfc();

    auto renderTxt = [&](GsSurface &textSfc)
    {
        if(!textSfc.empty())
        {
            const auto textW = textSfc.width();
            const auto textH = textSfc.height();
            GsVec2D<int> textSfcDim(textW, textH);

            GsRect<float> blitPos = displayRect;
            blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;

            textSfc.blitTo(blit, blitPos.SDLRect());
        }
    };

    auto &textSfcVec = mTextSfcVecByColor[mTextColor];

    if(mTransition)
    {
        auto &sfcPrev = textSfcVec[mPreLineIdx];
        auto &sfcCur  = textSfcVec[mCurLineIdx];

        sfcPrev.setAlpha(255-alpha);
        sfcCur.setAlpha(alpha);

        renderTxt( sfcPrev );
        renderTxt( sfcCur );
    }
    else
    {
        renderTxt( textSfcVec[mCurLineIdx] );
    }

/*
    unsigned int totTextSfcH = 0;
    for(auto &textSfc : mTextSfcVec)
    {
        if(textSfc.empty())
            break;

        const auto textW = textSfc.width();
        const auto textH = textSfc.height();
        GsVec2D<int> textSfcDim(textW, textH);

        GsRect<float> blitPos = displayRect;
        blitPos.pos = blitPos.pos + (blitPos.dim-textSfcDim)/2;
        blitPos.pos.y += totTextSfcH;

        textSfc.blitTo(blit, blitPos.SDLRect());
        totTextSfcH += textH;
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
