/*
 * CGUIButton.cpp
 *
 *  Created on: 26.11.2011
 *      Author: gerstrong
 */

#include "CGUIButton.h"
#include "graphics/CGfxEngine.h"
#include "sdl/input/CInput.h"
#include "sdl/input/InputEvents.h"
#include "sdl/CVideoDriver.h"
#include "core/mode/CGameMode.h"
#include "sdl/CTimer.h"
#include "StringUtils.h"


CGUIButton::CGUIButton(	const std::string& text,
						const SmartPointer<CEvent> ev,
						const Style style ) :
mText(text),
mEvent(ev),
drawButton(&CGUIButton::drawNoStyle)
{
	mMapping[UNSET] 	= g_pBehaviorEngine->getEngine();
	mMapping[NONE] 		= ENGINE_LAUNCHER;
	mMapping[VORTICON]	= ENGINE_VORTICON;
	mMapping[GALAXY] 	= ENGINE_GALAXY;


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
	CFont &Font = g_pGfxEngine->getFont(mFontID);
	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;

	mpTextDarkSfc = Font.fetchColoredTextSfc(mText, SDL_MapRGB( format, 38, 134, 38));
	mpTextLightSfc = Font.fetchColoredTextSfc(mText, SDL_MapRGB( format, 84, 234, 84));
	mpTextDisabledSfc = Font.fetchColoredTextSfc(mText, SDL_MapRGB( format, 123, 123, 123));
}


void CGUIButton::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		g_pBehaviorEngine->m_EventList.add(mEvent);
	}
}


void CGUIButton::processLogic()
{
	if(!mEnabled)
		return;

	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( MouseMoveEvent *mouseevent = g_pInput->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
			if(mouseevent->Type == MOUSEEVENT_MOVED)
			{
				mHovered = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
			{
				mButtonDown = true;
				g_pInput->m_EventList.pop_Event();
			}
			else if(mouseevent->Type == MOUSEEVENT_BUTTONUP)
			{
				mButtonUp = true;
				g_pBehaviorEngine->m_EventList.add(mEvent);
				g_pInput->m_EventList.pop_Event();
			}
		}
		else
		{

			mHovered = false;
			mButtonDown = false;
			mButtonUp = false;
		}
	}
}


void CGUIButton::drawVorticonStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;


	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the button
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );

	drawTwirl(lRect);

}


void CGUIButton::drawGalaxyStyle(SDL_Rect& lRect)
{
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if(!mEnabled)
	{
		SDL_BlitSurface(mpTextDisabledSfc.get(), NULL, blitsfc, &lRect);
		return;
	}

	if(mHovered)
	{
		SDL_BlitSurface(mpTextLightSfc.get(), NULL, blitsfc, &lRect);
	}
	else // Button is not hovered
	{
		SDL_BlitSurface(mpTextDarkSfc.get(), NULL, blitsfc, &lRect);
	}
}


void CGUIButton::drawNoStyle(SDL_Rect& lRect)
{
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if( mButtonUp )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00CFCFCF );
	}
	else if( mButtonDown )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00DFDFDF );
	}
	else if( mHovered )
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00EFEFEF );
	}
	else
	{
		drawRect( blitsfc, &lRect, 1, 0x00BBBBBB, 0x00FFFFFF );
	}

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	Font.drawFontCentered( blitsfc, mText, lRect.x, lRect.w, lRect.y, lRect.h,false );
}


void CGUIButton::processRender(const CRect<float> &RectDispCoordFloat)
{
	// Transform to the display coordinates
	CRect<float> displayRect = mRect;
	displayRect.transform(RectDispCoordFloat);
	SDL_Rect lRect = displayRect.SDLRect();

	(this->*drawButton)(lRect);
}
