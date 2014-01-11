/*
 * GsButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include <graphics/GsGraphics.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include <widgets/GsMenuController.h>
#include <base/PointDevice.h>

#include "GsButton.h"


GsButton::GsButton(const std::string& text,
			CEvent *ev,
			const Style style) :
mText(text),
mEvent(ev)
{}

bool GsButton::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		gEventManager.add(mEvent);
		return true;
	}
	return false;
}


void GsButton::updateGraphics()
{
    if( auto menuEv = std::dynamic_pointer_cast<OpenMenuEvent>(mEvent)  )
    {
        menuEv->mMenuDialogPointer->updateGraphics();
    }
}


void GsButton::processLogic()
{        
    processPointingState();

    // If button was pushed and gets released, trigger the assigned event.
    if(mReleased)
    {
        gEventManager.add(mEvent);
    }
}



void GsButton::drawNoStyle(SDL_Rect& lRect)
{
    if(!mEnabled) // TODO: I think, if it is disabled, it should use another color
        return;

    SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

    if( mHovered )
    {
        if( mPressed )
        {
            drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFCFCFCF );
        }
        else
        {
            drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFDFDFDF );
        }
    }
    else
    {
        drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFFFFFFF );
    }

	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );
}


void GsButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    drawNoStyle(lRect);
}
