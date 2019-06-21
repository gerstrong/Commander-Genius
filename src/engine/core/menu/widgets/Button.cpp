
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GameButton::GameButton(const std::string& text,
                       CEvent *ev,
                       const Style style) :
GsButton(text, ev),
mStyle(style)
{
    if(mStyle == Style::GALAXY)
    {
        enableBorder(false);
        enableCenteringH(false);
        enableBlinker(true);

        mColorNormal   = GsColor(0x26, 0x86, 0x26);
        mColorHovered  = GsColor(0x66, 0xC6, 0x66);
        mColorPressed  = GsColor(0x66, 0xF6, 0x66);
        mColorReleased  = GsColor(0x66, 0xF6, 0x66);
        mColorSelected = GsColor(0xA6, 0xC6, 0x66);

        setFontId(1);
        setText(text);
    }
    else if(mStyle == Style::VORTICON)
    {
        setFontId(0);
    }
}

GameButton::GameButton(const std::string& text,
                       const GsRect<float> &rect,
                       CEvent *ev,
                       const Style style) :
GsButton(text, rect, ev),
mStyle(style)
{
    if(mStyle == Style::GALAXY)
    {
        setText(text);
        setFontId(1);
    }
    else if(mStyle == Style::VORTICON)
    {
        setFontId(0);
    }
}

void GameButton::processLogic()
{
    GsButton::processLogic();

    if(mHovered)
    {
        mTextWidget.setTextColor( mColorHovered );
    }
    else if(mPressed)
    {
        mTextWidget.setTextColor( mColorPressed );
    }
    else if(mReleased)
    {
        mTextWidget.setTextColor( mColorReleased );
    }
    else if(mSelected)
    {
        mTextWidget.setTextColor( mColorSelected );
    }
    else
    {
        mTextWidget.setTextColor( mColorNormal );
    }
}


void GameButton::processRender(const GsRect<float> &RectDispCoordFloat)
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

    GsButton::processRender(controlsRect);
}
