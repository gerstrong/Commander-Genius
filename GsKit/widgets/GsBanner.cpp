#include "GsBanner.h"

#include <graphics/GsGraphics.h>
#include <base/video/CVideoDriver.h>


const int TIME_UNTIL_CHANGE = 180;
const int TIME_TRANSITION = 30;


CGUIBanner::CGUIBanner(const std::string& text) :
CGUIText(text),
transition(false),
timer(0)
{
    curTextIt = mTextList.begin();
}


void CGUIBanner::setText(const std::string& text)
{
    CGUIText::setText(text);
    curTextIt = mTextList.begin();    
}

void CGUIBanner::processLogic()
{        
    if(transition)
    {	
	if(timer >= TIME_TRANSITION)
	{
	    timer = 0;
	    alpha = 0;
	    transition = !transition;	
	    
	    return;
	}	
    }
    else
    {
	if(timer >= TIME_UNTIL_CHANGE)
	{
	    timer = 0;
	    
	    prevTextIt = curTextIt;
	    
	    curTextIt++;
	    
	    if( curTextIt == mTextList.end() )
		curTextIt = mTextList.begin();
	    
	    alpha = 0;
	    transition = !transition;	
	    
	    return;
	}	
    }
    
    alpha = ((255*timer)/TIME_TRANSITION);
    
    timer++;
    
}

void CGUIBanner::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	// Now lets draw the text of the list control
    GsFont &Font = gGraphics.getFont(mFontID);

	if(transition)
	{
	    Font.drawFontCenteredAlpha(gVideoDriver.getBlitSurface(), *prevTextIt, lRect.x, lRect.w, lRect.y, 255-alpha);
	    Font.drawFontCenteredAlpha(gVideoDriver.getBlitSurface(), *curTextIt, lRect.x, lRect.w, lRect.y, alpha);
	}
	else
	{
	    Font.drawFontCentered(gVideoDriver.getBlitSurface(), *curTextIt, lRect.x, lRect.w, lRect.y, false);
	}
	
}
