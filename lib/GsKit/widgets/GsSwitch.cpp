/*
 * CGUISwitch.cpp
 *
 *  Created on: 04.03.2012
 *      Author: gerstrong
 */

#include <base/utils/StringUtils.h>
#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>


#include "GsSwitch.h"

//#include "engine/core/CBehaviorEngine.h"

CGUISwitch::CGUISwitch( const std::string& text ) :
CGUIComboSelection( text, filledStrList( 2, "off", "on" ) )
{

}

void CGUISwitch::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(2);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
	Font.drawFont( blitsfc, ":", lRect.x+24+mText.size()*8, lRect.y, false );

	std::string text;
	if( (*mOLCurrent) == "off" )
		text = "\24\25\26\27";
	else
		text = "\34\35\36\37";


	Font.drawFont( blitsfc, text, lRect.x+24+(mText.size()+2)*8, lRect.y, false );

	drawTwirl(lRect);

}

void CGUISwitch::processRender(const GsRect<float> &RectDispCoordFloat)
{

	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
/*	SDL_Rect lRect = displayRect.SDLRect();

    if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		drawVorticonStyle(lRect);
	}
	else
	{
		(this->*drawButton)(lRect);
	}
*/
}
