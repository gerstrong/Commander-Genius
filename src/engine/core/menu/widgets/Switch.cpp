#include "Switch.h"

#include "base/video/CVideoDriver.h"
#include "graphics/GsGraphics.h"

#include <base/utils/StringUtils.h>


Switch::Switch( const std::string& text, 
                const Style style ) :
ComboSelection( text, filledStrList( 2, "off", "on" ), style )
{
}

void Switch::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
    {
		return;
    }

	drawTwirl(lRect);
}

void Switch::drawNoStyle(SDL_Rect& )
{
    if(!mEnabled)
    {
        return;
    }
}
