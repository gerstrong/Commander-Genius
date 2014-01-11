/*
 * CGUIButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include "CGUIButton.h"
#include "graphics/GsGraphics.h"
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include "core/mode/CGameMode.h"
#include <base/GsTimer.h>
#include "sdl/extensions.h"
#include <widgets/CMenuController.h>
#include <base/PointDevice.h>



CGUIButton::CGUIButton(const std::string& text,
			CEvent *ev,
			const Style style) :
mText(text),
mEvent(ev),
drawButton(&CGUIButton::drawNoStyle)
{
	mMapping[UNSET] 	= g_pBehaviorEngine->getEngine();
	mMapping[NONE] 		= ENGINE_LAUNCHER;
	mMapping[VORTICON]	= ENGINE_VORTICON;
	mMapping[GALAXY] 	= ENGINE_GALAXY;
	mMapping[GALAXY_BORDERED] = ENGINE_GALAXY;

	switch( mMapping[style] )
	{

	case ENGINE_VORTICON:
	{
		mFontID = 1;
		drawButton = &CGUIButton::drawVorticonStyle;
		break;
	}

	case ENGINE_GALAXY:
	{
		mFontID = 1;
		if(style == GALAXY_BORDERED)
			drawButton = &CGUIButton::drawGalaxyBorderedStyle;
		else
			drawButton = &CGUIButton::drawGalaxyStyle;
		setupButtonSurface();
		break;
	}

	default:
	{
		mFontID = 0;
		drawButton = &CGUIButton::drawNoStyle;
		break;
	}

	}

}

void CGUIButton::setupButtonSurface()
{
	if( drawButton == &CGUIButton::drawNoStyle  )
		return;

	GsFont &Font = gGraphics.getFont(mFontID);
	SDL_PixelFormat *format = gVideoDriver.getBlitSurface()->format;

	mpTextDarkSfc.reset(Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 38, 134, 38)));
	mpTextLightSfc.reset(Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 84, 234, 84)));
	mpTextDisabledSfc.reset(Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 123, 150, 123)));

}


bool CGUIButton::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		gEventManager.add(mEvent);
		return true;
	}
	return false;
}


void CGUIButton::updateGraphics()
{
    if( auto menuEv = std::dynamic_pointer_cast<OpenMenuEvent>(mEvent)  )
    {
        menuEv->mMenuDialogPointer->updateGraphics();
    }
}


void CGUIButton::processLogic()
{        
    processPointingState();

    // If button was pushed and gets released, trigger the assigned event.
    if(mReleased)
    {
        gEventManager.add(mEvent);
    }
}


void CGUIButton::drawVorticonStyle(SDL_Rect& lRect)
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



void CGUIButton::drawGalaxyBorderedStyle(SDL_Rect& lRect)
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


void CGUIButton::drawGalaxyStyle(SDL_Rect& lRect)
{
	SDL_Surface *blitsfc = gVideoDriver.getBlitSurface();

	if(!mEnabled)
	{
		SDL_BlitSurface(mpTextDisabledSfc.get(), NULL, blitsfc, &lRect);
	}
	else
	{
		if(mHovered)
		{
			SDL_BlitSurface(mpTextLightSfc.get(), NULL, blitsfc, &lRect);
		}
		else // Button is not hovered
		{
			SDL_BlitSurface(mpTextDarkSfc.get(), NULL, blitsfc, &lRect);
		}
	}

	drawBlinker(lRect);
}


void CGUIButton::drawNoStyle(SDL_Rect& lRect)
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


void CGUIButton::processRender(const GsRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
    GsRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);
}
