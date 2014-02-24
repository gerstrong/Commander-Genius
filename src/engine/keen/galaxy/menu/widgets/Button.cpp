
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
}




void GalaxyButton::drawEnabledButton(GsWeakSurface &blitsfc, const SDL_Rect& lRect)
{
    mTextLightSfc.setAlpha(mLightRatio);
    mTextDarkSfc.setAlpha(255-mLightRatio);

    if(mLightRatio > 0)
    {
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
        drawEnabledButton(blitsfc, lRect);
    }

    drawBlinker(lRect);
}
