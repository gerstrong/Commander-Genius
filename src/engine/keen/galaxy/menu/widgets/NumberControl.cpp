
#include "NumberControl.h"

#include <base/video/CVideoDriver.h>

namespace galaxy
{


NumberControl::NumberControl(const std::string &text,
                             const int startValue,
                             const int endValue,
                             const int deltaValue,
                             const int value,
                             const bool slider) :
CGUINumberControl(text, startValue, endValue,
                    deltaValue, value, slider)
{

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
        if(mHovered)
        {
            if(mDecSel)
                mTextLightSfcL.blitTo(blitsfc, lRect);
            else if(mIncSel)
                mTextLightSfcR.blitTo(blitsfc, lRect);
            else
                mTextLightSfc.blitTo(blitsfc, lRect);
        }
        else // Button is not hovered
        {
            mTextDarkSfc.blitTo(blitsfc, lRect);
        }
    }

    drawBlinker(lRect);
}

}
