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


const int BLEND_SPEED = 16;


GsButton::GsButton(const std::string& text,
			CEvent *ev,
			const Style style) :
mText(text),
mLightRatio(128),
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

    if(mEnabled)
    {
        // For some nice special effects
        if(mHovered || mSelected)
        {
            if(mLightRatio+BLEND_SPEED < 255)
               mLightRatio += BLEND_SPEED;
            else
               mLightRatio = 255;
        }
        else // Button is not hovered
        {
            if(mLightRatio-BLEND_SPEED > 0)
               mLightRatio -= BLEND_SPEED;
            else
               mLightRatio = 0;
        }

        // If button was pushed and gets released, trigger the assigned event.
        if(mReleased)
        {
            gEventManager.add(mEvent);
        }
    }
}

void GsButton::drawNoStyle(SDL_Rect& lRect)
{        
    GsWeakSurface blitsfc(gVideoDriver.getBlitSurface());

    int lComp;

    if(mEnabled)
    {
        if( mPressed || mSelected )
            lComp = 0xFF - (mLightRatio*(0xFF-0xCF)/255);
        else
            lComp = 0xFF - (mLightRatio*(0xFF-0xDF)/255);
    }
    else
    {
        lComp = 0xFF;
    }

    const Uint32 fillColor = blitsfc.mapRGBA( lComp, lComp, lComp, 0xFF);

    GsRect<Uint16> rect(lRect);

    blitsfc.drawRect( rect, 1, 0xFFBBBBBB, fillColor );


	// Now lets draw the text of the list control
	GsFont &Font = gGraphics.getFont(mFontID);

    if(mEnabled) // If the button is enabled use the normal text, otherwise the highlighted color
        Font.drawFontCentered( blitsfc.getSDLSurface(), mText, lRect.x, lRect.w, lRect.y, lRect.h, false );
    else
        Font.drawFontCentered( blitsfc.getSDLSurface(), mText, lRect.x, lRect.w, lRect.y, lRect.h, true );
}

void GsButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;

    displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

    drawNoStyle(lRect);
}
