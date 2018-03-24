
#include <sstream>
#include <iomanip>

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "GsProgressbar.h"

void GsProgressBar::processLogic()
{
    auto fProgress = float(mProgress);

    // Note: So far no logic required. Usually the a reference to mProgress the variable sets the progress
    auto diff = fProgress - mProgressToRender;
    const float tol = 1.0f; // Tolerance
    const float acc = 0.3f; // Acceleration

    if( abs(diff) > tol )
    {
        mProgressToRender = mProgressToRender + diff*acc;
    }
    else
    {
        mProgressToRender = mProgress;
    }

    // Animation progress fuel

    mAnimationPos ++;
    if(mAnimationPos > 2.0*mFuelAnimationWidth)
    {
        mAnimationPos = 0;
    }



    // If finished downloading flashy animation and nice timer
    if( mProgressToRender >= 1000.0 ) // Finished
    {
        mFlashTimer--;

        // Here Progress tells that it finished!
        if(mFlashTimer < 0)
        {
            mFinished = true;
        }

        if(mFlashAmt > 0)
        {
           mFlashBlue += 10;

           if(mFlashBlue >= 255)
           {
               mFlashBlue = 255;
               mFlashAmt = -1;
           }
        }
        else if(mFlashAmt < 0)
        {
           mFlashBlue -= 10;

           if(mFlashBlue < 0)
           {
               mFlashBlue = 0;
               mFlashAmt = 0;
           }
        }

    }

}

void
GsProgressBar::applyFancyAnimation(SDL_Surface *pBlitsurface,
                                   const SDL_Rect &progressSDLRect,
                                   const Uint32 color1,
                                   const Uint32 color2)
{
    SDL_Rect progressSDLRectStripe = progressSDLRect;


    auto curColor = color1;

    const auto endX = progressSDLRect.x+progressSDLRect.w;

    // Only apply the effect if there is enough space otherwise no animation
    if(progressSDLRect.w < 2.0*mFuelAnimationWidth)
    {
        SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);

        return;
    }

    // First fuel part of the progress bar

    // Ff position larger than mFuelAnimationWidth we will
    // need two with alternating color
    if(mAnimationPos > mFuelAnimationWidth)
    {
        progressSDLRectStripe.w = mAnimationPos-mFuelAnimationWidth;

        SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);

        // Switch color
        curColor = (curColor == color1) ? color2 : color1;

        progressSDLRectStripe.x += progressSDLRectStripe.w;

        progressSDLRectStripe.w = mFuelAnimationWidth;
        SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);
    }
    else
    {
        // Switch color
        curColor = (curColor == color1) ? color2 : color1;

        progressSDLRectStripe.w = mAnimationPos;

        SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);
    }


    progressSDLRectStripe = progressSDLRect;

    progressSDLRectStripe.w = mFuelAnimationWidth;
    progressSDLRectStripe.x += mAnimationPos;

    // In the middle
    for( ; progressSDLRectStripe.x < endX-mFuelAnimationWidth ;
         progressSDLRectStripe.x += mFuelAnimationWidth)
    {
        // Switch color in the loop
        curColor = (curColor == color1) ? color2 : color1;

        SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);
    }

    // Switch for the last time
    curColor = (curColor == color1) ? color2 : color1;

    progressSDLRectStripe.w = endX - progressSDLRectStripe.x;

    SDL_FillRect(pBlitsurface, &progressSDLRectStripe, curColor);


}


void
GsProgressBar::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    GsRect<float> progressRect = mRect;

    auto pBlitsurface = gVideoDriver.getBlitSurface();

    float progressWidth = 0.0;

    if(mProgressToRender < 0.0 || mProgressToRender > 1000.0)
    {
        progressWidth = displayRect.w;
    }
    else
    {
        progressWidth = (float(mProgressToRender)*displayRect.w)/1000.0f;
    }

    progressRect.w = progressWidth;


    displayRect.transform(RectDispCoordFloat);
    progressRect.transform(RectDispCoordFloat);

    SDL_Rect bgSDLRect = displayRect.SDLRect();
    SDL_Rect progressSDLRect = progressRect.SDLRect();

    const auto bgColor = SDL_MapRGBA( pBlitsurface->format, 240, 255, 240, 255 );

    auto progressFloat = float(mProgressToRender)/10.0f;

    std::stringstream ss;

    if( mProgressToRender == 0.0 ) // Less than not doing
    {
        ss << "0 %";

        if(mDoFancyAnimation)
        {
            // Two colors for fuel animation
            const auto progressColor1 = SDL_MapRGBA( pBlitsurface->format, 210, 210, 210, 255 );
            const auto progressColor2 = SDL_MapRGBA( pBlitsurface->format, 180, 180, 180, 255 );

            applyFancyAnimation(pBlitsurface, bgSDLRect, progressColor1, progressColor2);
        }
        else
        {
            SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);
        }
    }
    else if( mProgressToRender < 0.0 ) // Less than not doing
    {
        const auto progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, 128, 255 );

        ss << "Fetching ...";

        SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);
        SDL_FillRect(pBlitsurface, &progressSDLRect, progressColor);
    }
    else if( mProgressToRender >= 1000.0 ) // Finished
    {
        SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);

        if(!mBad)
        {
            const auto progressColor = SDL_MapRGBA( pBlitsurface->format, 0, 255, mFlashBlue, 255 );
            ss << "Completed!";
            SDL_FillRect(pBlitsurface, &progressSDLRect, progressColor);
        }
        else
        {
            const auto progressColor = SDL_MapRGBA( pBlitsurface->format, 255, 0, mFlashBlue, 255 );
            ss << "Aborted!";
            SDL_FillRect(pBlitsurface, &progressSDLRect, progressColor);
        }
    }
    else if( mProgressToRender > 0.0 ) // Doing progress
    {
        // Two colors for fuel animation
        const auto progressColor1 = SDL_MapRGBA( pBlitsurface->format, 0, 255, 0, 255 );
        const auto progressColor2 = SDL_MapRGBA( pBlitsurface->format, 120, 255, 90, 255 );

        ss << "Now at ";
        ss << std::setprecision(1);
        ss << std::fixed;
        ss << progressFloat;
        ss << " %";

        SDL_FillRect(pBlitsurface, &bgSDLRect, bgColor);

        applyFancyAnimation(pBlitsurface, progressSDLRect, progressColor1, progressColor2);
    }

    // Now lets draw the text of the list control
    auto &Font = gGraphics.getFont(mFontID);

    const auto fontHeight = Font.getPixelTextHeight();
    const auto fontMidYPos = bgSDLRect.y + (progressSDLRect.h-fontHeight)/2;

    Font.drawFontCentered( pBlitsurface, ss.str(), bgSDLRect.x, bgSDLRect.w, fontMidYPos, false );
}
