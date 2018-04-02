#include "Switch.h"

#include "base/video/CVideoDriver.h"
#include "graphics/GsGraphics.h"

#include <base/utils/StringUtils.h>


Switch::Switch( const std::string& text, 
                const GsControl::Style style ) :
ComboSelection( text, filledStrList( 2, "off", "on" ), style )
{
}

void Switch::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
    {
		return;
    }

	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();
       
	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(2);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );

	std::string text;
	if( (*mOLCurrent) == "off" )
    {
		text = "\24\25\26\27";
    }
	else
    {
		text = "\34\35\36\37";
    }


	Font.drawFont( blitsfc, text, 
                   lRect.x+24+(mText.size()+2)*8, 
                   lRect.y, false );

	drawTwirl(lRect);
}

void Switch::drawNoStyle(SDL_Rect& lRect)
{
    if(!mEnabled)
    {
        return;
    }

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    // Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(0);

    std::string text = mText + ":" + (*mOLCurrent);

    Font.drawFont( blitsfc, text, lRect.x+40, lRect.y, false );

    drawTwirl(lRect);
}

void Switch::processRender(const GsRect<float> &RectDispCoordFloat)
{
    if(mStyle == Style::GALAXY)
    {
        ComboSelection::processRender(RectDispCoordFloat);
    }
    else if(mStyle == Style::VORTICON)
    {
        // Transform to the display coordinates
        GsRect<float> displayRect = mRect;
        displayRect.transform(RectDispCoordFloat);
        SDL_Rect lRect = displayRect.SDLRect();
        drawVorticonStyle(lRect);
    }
    else
    {
        GsRect<float> displayRect = mRect;
        displayRect.transform(RectDispCoordFloat);
        SDL_Rect lRect = displayRect.SDLRect();
        drawNoStyle(lRect);
    }
}
