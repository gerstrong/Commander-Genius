
#include "NumberControl.h"

#include <base/video/CVideoDriver.h>
#include <base/utils/StringUtils.h>
#include <graphics/GsGraphics.h>

namespace galaxy
{


NumberControl::NumberControl(const std::string &text,
                             const int startValue,
                             const int endValue,
                             const int deltaValue,
                             const int value) :
CGUINumberControl(text, startValue, endValue,
                    deltaValue, value, 1,false)
{
    setupButtonSurface();
}


void NumberControl::setupButtonSurface()
{        
    GsFont &Font = gGraphics.getFont(mFontID);

    const std::string showText = "  " + mText + ": " + itoa(mValue);
    GsButton::setupButtonSurface(showText);

    const std::string showTextL = "  " + mText + ":<" + itoa(mValue);
    const std::string showTextR = "  " + mText + ": " + itoa(mValue) + ">";

    Font.createTextSurface(mTextLightSfcR, showTextR, 84, 234, 84 );
    Font.createTextSurface(mTextLightSfcL, showTextL, 84, 234, 84 );
}


void NumberControl::processLogic()
{
    CGUINumberControl::processLogic();

    if(mMustRedraw)
    {
        mMustRedraw = false;
        setupButtonSurface();
    }
}

void NumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(blitsfc, lRect);
    }
    else
    {
        drawEnabledButton(blitsfc, lRect, mHovered);

        if(mHovered)
        {
            if(mDecSel)
                mTextLightSfcL.blitTo(blitsfc, lRect);
            else if(mIncSel)
                mTextLightSfcR.blitTo(blitsfc, lRect);
            else
                mTextLightSfc.blitTo(blitsfc, lRect);
        }
    }

    drawBlinker(lRect);
}

}
