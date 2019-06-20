
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GameButton::GameButton(const std::string& text,
                       CEvent *ev,
                       const Style style) :
GsButton(text, ev, style)
{
    if(mStyle == Style::GALAXY)
    {
        enableBorder(false);
        enableCenteringH(false);
        enableBlinker(true);

        setTextColor(GsColor(0x26, 0x86, 0x26));
        setTextColorHovered(GsColor(0x66, 0xC6, 0x66));
        setTextColorPressed(GsColor(0x66, 0xF6, 0x66));
        setTextColorSelected(GsColor(0xA6, 0xC6, 0x66));

        mFontID = 1;
        setText(text);
    }
    else if(mStyle == Style::VORTICON)
    {
        mFontID = 1;
    }
}

GameButton::GameButton(const std::string& text,
                       const GsRect<float> &rect,
                       CEvent *ev,
                       const Style style) :
GsButton(text, rect, ev, style)
{
    if(mStyle == Style::GALAXY)
    {
        mFontID = 1;
        setText(text);
    }
    else if(mStyle == Style::VORTICON)
    {
        mFontID = 1;
    }
}


void GameButton::drawVorticonStyle(SDL_Rect& lRect)
{
    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the button
    if(mEnabled)
    {
        auto &Font = gGraphics.getFontLegacy(mFontID);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
        drawTwirl(lRect);
    }
    else
    {
        auto &Font = gGraphics.getFontLegacy(0);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, true );
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
