#include "InputText.h"

#include <base/video/CVideoDriver.h>
#include <graphics/GsFont.h>
#include <graphics/GsGraphics.h>



InputText::InputText(const std::string &text,
          const GsRect<float> &rect,
          const Style style) :
CGUIInputText(text, rect, -1),
mStyle(style)
{
    setupStyle();
}

void InputText::setupStyle()
{
    if(mStyle == Style::GALAXY)
    {
        enableBorder(false);
        enableCenteringH(false);
        enableBlinker(true);

        mColorNormal   = GsColor(0x26, 0x86, 0x26);
        mColorHovered  = GsColor(0x66, 0xC6, 0x66);
        mColorPressed  = GsColor(0x66, 0xF6, 0x66);
        mColorReleased = GsColor(0x46, 0xF6, 0x56);
        mColorSelected = GsColor(0xA6, 0xC6, 0x66);
        mColorDisabled = GsColor(0xA6, 0xA6, 0xA6);

        setFontId(1);
        setText(mText);
    }
    else if(mStyle == Style::VORTICON)
    {
        enableBorder(false);
        enableCenteringH(false);
        enableTwirl(true);

        mColorNormal   = GsColor(0x0, 0x0, 0x0);
        mColorHovered  = GsColor(0x11, 0x11, 0x11);
        mColorPressed  = GsColor(0x44, 0x44, 0x44);
        mColorReleased = GsColor(0x33, 0x33, 0x33);
        mColorSelected = GsColor(0x22, 0x22, 0x22);
        mColorDisabled = GsColor(0xA6, 0xA6, 0xA6);

        setFontId(0);
        setText(mText);
    }
}

void InputText::processLogic()
{
    GsButton::processLogic();

    mTextWidget.setTextColor( mColorNormal );

    if(mHovered)
    {
        mTextWidget.setTextColor( mColorHovered );
    }

    if(mPressed)
    {
        mTextWidget.setTextColor( mColorPressed );
    }

    if(mReleased)
    {
        mTextWidget.setTextColor( mColorReleased );
    }

    if(mSelected)
    {
        mTextWidget.setTextColor( mColorSelected );
    }

    if(!mEnabled)
    {
        mTextWidget.setTextColor( mColorDisabled );
    }
}

void InputText::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = getRect();
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

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

    CGUIInputText::processRender(controlsRect);
}
