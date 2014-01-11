
#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>

#include "VorticonButton.h"

VorticonButton::VorticonButton(const std::string& text,
             CEvent *ev) :
GsButton(text, ev)
{
    mFontID = 1;
}


void VorticonButton::setupButtonSurface()
{
    GsFont &Font = gGraphics.getFont(mFontID);
    SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

    mTextDarkSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 38, 134, 38));
    mTextLightSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 84, 234, 84));
    mTextDisabledSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 123, 150, 123));
}


void VorticonButton::drawVorticonStyle(SDL_Rect& lRect)
{
    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the button
    if(mEnabled)
    {
        GsFont &Font = gGraphics.getFont(mFontID);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
        drawTwirl(lRect);
    }
    else
    {
        GsFont &Font = gGraphics.getFont(0);
        Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, true );
    }
}


void VorticonButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    drawVorticonStyle(lRect);
}
