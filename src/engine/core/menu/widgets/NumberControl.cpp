
#include "NumberControl.h"

#include <base/video/CVideoDriver.h>
#include <base/interface/StringUtils.h>
#include <graphics/GsGraphics.h>

#include "widgets/GsMenuController.h"

NumberControl::NumberControl(const std::string &text,
                             const int startValue,
                             const int endValue,
                             const int deltaValue,
                             const int value,
                             const bool slider,
                             const Style style) :
CGUINumberControl(text,
                  GsRect<float>(0.0f,0.0f,1.0f,1.0f),
                  startValue, endValue,
                  deltaValue, value,
                  slider),
mStyle(style)
{
    if(mStyle == Style::GALAXY)
    {
        enableButtonBorders(false);
        enableWidgetsCenteringH(false);
        enableBlinker(true);
        enableHighlightBg(false);

        mColorNormal   = GsColor(0x26, 0x86, 0x26);
        mColorHovered  = GsColor(0x66, 0xC6, 0x66);
        mColorPressed  = GsColor(0x66, 0xF6, 0x66);
        mColorReleased = GsColor(0x46, 0xF6, 0x56);
        mColorSelected = GsColor(0xA6, 0xC6, 0x66);

        setTextColor(mColorNormal);

        mpCtrlName->setFontId(1);
        mpLeftButton->setFontId(1);
        mpCtrlValue->setFontId(1);
        mpRightButton->setFontId(1);
    }

    if(mStyle == Style::VORTICON)
    {
        enableTwirl(true);
        enableHighlightBg(false);

        mColorNormal   = GsColor(0x0, 0x0, 0x0);
        mColorHovered  = GsColor(0x11, 0x11, 0x11);
        mColorPressed  = GsColor(0x44, 0x44, 0x44);
        mColorReleased = GsColor(0x33, 0x33, 0x33);
        mColorSelected = GsColor(0x22, 0x22, 0x22);

        mpCtrlName->setFontId(0);
        mpLeftButton->setFontId(0);
        mpCtrlValue->setFontId(0);
        mpRightButton->setFontId(0);
    }
    else // No style, like the game launcher does use
    {
        mSelectedColor = GsColor(0x52, 0xfb, 0x52);
        enableTwirl(gMenuController.mEnableTwirl);
    }
}


void NumberControl::processLogic()
{
    setTextColor(mColorNormal);

    if(mHovered)
    {
        setTextColor(mColorHovered);
    }
    if(mSelected)
    {
        setTextColor(mColorSelected);
    }

    CGUINumberControl::processLogic();
}

void NumberControl::processRender(const GsRect<float> &RectDispCoordFloat)
{    
    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    auto controlsRect = RectDispCoordFloat;

    if(mDrawBlinker)
    {
        drawBlinker(lRect);
        controlsRect.pos.x += 11;
        controlsRect.dim.x -= 11;
    }

    if(mDrawTwirl)
    {
        controlsRect.pos.x += 24;
        controlsRect.dim.x -= 24;
        drawTwirl(lRect);
    }

    CGUINumberControl::processRender(controlsRect);
}

