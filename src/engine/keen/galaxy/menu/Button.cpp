
#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>

#include "Button.h"


GalaxyButton::GalaxyButton(const std::string& text,
                           CEvent *ev,
                           const bool bordered) :
GsButton(text, ev),
mBordered(bordered)
{
    mFontID = 1;

/*    if(style == GALAXY_BORDERED)
        drawButton = &GsButton::drawGalaxyBorderedStyle;
    else
        drawButton = &GsButton::drawGalaxyStyle;*/
    setupButtonSurface();
}


void GalaxyButton::setupButtonSurface()
{
    GsFont &Font = gGraphics.getFont(mFontID);
    SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

    mTextDarkSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 38, 134, 38));
    mTextLightSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 84, 234, 84));
    mTextDisabledSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 123, 150, 123));
}


void GalaxyButton::drawGalaxyBorderedStyle(SDL_Rect& lRect)
{
    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

    SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

    const Uint32 oldcolor = Font.getFGColor();

    Uint32 newcolor;

    if(!mEnabled)
        newcolor = SDL_MapRGB( format, 123, 150, 123);
    else if(mHovered || mPressed)
        newcolor = SDL_MapRGB( format, 84, 234, 84);
    else
        newcolor = SDL_MapRGB( format, 38, 134, 38);

    Font.setupColor( newcolor );

    drawEmptyRect( blitsfc, &lRect, newcolor);

    Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y+2, false );

    Font.setupColor( oldcolor );
}



void GalaxyButton::drawGalaxyStyle(SDL_Rect& lRect)
{
    GsSurface blitsfc( gVideoDriver.getBlitSurface() );
    blitsfc.disownSfc();

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(blitsfc, lRect);
    }
    else
    {
        if(mHovered)
        {
            mTextLightSfc.blitTo(blitsfc, lRect);
        }
        else // Button is not hovered
        {
            mTextDarkSfc.blitTo(blitsfc, lRect);
        }
    }

    drawBlinker(lRect);
}

void GalaxyButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
    // Transform to the display coordinates
    GsRect<float> displayRect = mRect;
    displayRect.transform(RectDispCoordFloat);
    SDL_Rect lRect = displayRect.SDLRect();

    drawGalaxyStyle(lRect);
}
