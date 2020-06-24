#include "InputText.h"

#include <base/video/CVideoDriver.h>
#include <graphics/GsFont.h>
#include <graphics/GsGraphics.h>



InputText::InputText(const std::string &text,
          const GsRect<float> &rect,
          const int index,
          const Style style) :
CGUIInputText(text, rect, -1),
mStyle(style),
mIndex(index)
{
    setupStyle();
}

void InputText::setupStyle()
{
    if(mStyle == Style::GALAXY)
    {
        enableBorder(false);
        enableCenteringH(true);
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
        enableCenteringH(true);
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
    CGUIInputText::processLogic();

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

    if(mDrawTwirl)
    {
        controlsRect.pos.x += 24;
        controlsRect.dim.x -= 24;
        drawTwirl(lRect);
    }
    else
    {
        controlsRect.pos.x += 5;
        controlsRect.dim.x -= 5;
    }

    CGUIInputText::processRender(controlsRect);

    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    Uint32 newcolor;

    if(!mEnabled)
        newcolor = blitsfc.mapRGB(123, 150, 123);
#ifndef DISABLE_HOVER
    else if(mHovered || mPressed)
        newcolor = blitsfc.mapRGB(84, 234, 84);
#else
    else if(mPressed)
        newcolor = blitsfc.mapRGB(84, 234, 84);
#endif
    else
        newcolor = blitsfc.mapRGB(38, 134, 38);

    blitsfc.drawFrameRect( lRect, 1, newcolor);

}
