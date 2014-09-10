
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GalaxyButton::GalaxyButton(const std::string& text,
                           CEvent *ev) :
GsButton(text, ev)
{
    mFontID = 1;

    setupButtonSurface();
}

void GalaxyButton::setupButtonSurface()
{
    GsFont &Font = gGraphics.getFont(mFontID);

    Font.createTextSurface(mTextDarkSfc, "  " + mText, 38, 134, 38 );
    Font.createTextSurface(mTextLightSfc, "  " + mText, 84, 234, 84 );
    Font.createTextSurface(mTextDisabledSfc, "  " + mText, 123, 150, 123 );
    Font.createTextSurface(mTextRedSfc, "  " + mText, 180, 50, 23 );
}




void GalaxyButton::drawEnabledButton(GsWeakSurface &blitsfc, const SDL_Rect& lRect, const bool alternate)
{
    mTextLightSfc.setAlpha(mLightRatio);
    mTextDarkSfc.setAlpha(255-mLightRatio);

    if(mLightRatio > 0)
    {
        if(alternate)
            mTextRedSfc.blitTo(blitsfc, lRect);
        else
            mTextLightSfc.blitTo(blitsfc, lRect);
    }

    if(mLightRatio < 255)
    {
        mTextDarkSfc.blitTo(blitsfc, lRect);
    }
}




void GalaxyButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(blitsfc, lRect);
    }
    else
    {
        if(mHovered)
            drawEnabledButton(blitsfc, lRect, true);
        else
            drawEnabledButton(blitsfc, lRect, false);
    }

    drawBlinker(lRect);
}
