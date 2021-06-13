
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GameButton::GameButton(const std::string& text,
                       CEvent *ev,
                       const Style style) :
GsButton(text, ev),
mStyle(style)
{
    setupStyle();
}

GameButton::GameButton(const std::string& text,
           const std::function <void ()>& f,
           const Style style) :
GsButton(text, f),
mStyle(style)
{
    setupStyle();
}

GameButton::GameButton(const std::string& text,
                       const GsRect<float> &rect,
                       CEvent *ev,
                       const Style style) :
GsButton(text, rect, ev),
mStyle(style)
{
    setupStyle();
}

void GameButton::setupStyle()
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

        mColorNormal   = GsColor(0x00, 0x00, 0x00);
        mColorHovered  = GsColor(0x11, 0x11, 0x11);
        mColorPressed  = GsColor(0x44, 0x44, 0x44);
        mColorReleased = GsColor(0x33, 0x33, 0x33);
        mColorSelected = GsColor(0x22, 0x22, 0x22);
        mColorDisabled = GsColor(0xA6, 0xA6, 0xA6);

        setFontId(0);
        setText(mText);
    }
}

void GameButton::processLogic()
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
