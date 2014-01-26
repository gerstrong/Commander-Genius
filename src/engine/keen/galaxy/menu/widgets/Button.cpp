
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

    Font.createTextSurface(mTextDarkSfc, "  " + mText, 38, 134, 38 );
    Font.createTextSurface(mTextLightSfc, "  " + mText, 84, 234, 84 );
    Font.createTextSurface(mTextDisabledSfc, "  " + mText, 123, 150, 123 );
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

void GalaxyButton::drawEnabledButton(GsWeakSurface &blitsfc, const SDL_Rect& lRect)
{
    mTextLightSfc.setAlpha(mLightRatio);
    mTextDarkSfc.setAlpha(255-mLightRatio);

    if(mLightRatio > 0)
    {
        mTextLightSfc.blitTo(blitsfc, lRect);
    }

    if(mLightRatio < 255)
    {
        mTextDarkSfc.blitTo(blitsfc, lRect);
    }
}


void GalaxyButton::drawGalaxyStyle(SDL_Rect& lRect)
{
    GsWeakSurface blitsfc( gVideoDriver.getBlitSurface() );

    if(!mEnabled)
    {
        mTextDisabledSfc.blitTo(blitsfc, lRect);
    }
    else
    {
        drawEnabledButton(blitsfc, lRect);
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
