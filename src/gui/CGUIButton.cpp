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
#include "sdl/extensions.h"
#include "StringUtils.h"



CGUIButton::CGUIButton(const std::string& text,
			CEvent *ev,
			const Style style) :
mText(text),
mEvent(ev),
drawButton(&CGUIButton::drawNoStyle),
mAutoActivation(false)
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

	CFont &Font = g_pGfxEngine->getFont(mFontID);
	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;

	mpTextDarkSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 38, 134, 38));
	mpTextLightSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 84, 234, 84));
	mpTextDisabledSfc = Font.fetchColoredTextSfc( "  " + mText, SDL_MapRGB( format, 123, 150, 123));

}


bool CGUIButton::sendEvent(const InputCommands command)
{
	if(command == IC_STATUS || command == IC_JUMP)
	{
		g_pBehaviorEngine->m_EventList.add(mEvent);
		return true;
	}
	return false;
}


void CGUIButton::processLogic()
{
	if(mAutoActivation)
	{
		g_pBehaviorEngine->m_EventList.add(mEvent);
	}


	// Here we check if the mouse-cursor/Touch entry clicked on our Button
	if( MouseMoveEvent *mouseevent = g_pInput->m_EventList.occurredEvent<MouseMoveEvent>() )
	{
		CVec MousePos = mouseevent->Pos;

		if( mRect.HasPoint(MousePos) )
		{
			if(mouseevent->Type == MOUSEEVENT_MOVED)
			{
				mHovered = true;

				if(!mEnabled)
					return;

				g_pInput->m_EventList.pop_Event();
				return;
			}


			if(!mEnabled)
				return;


			if(mouseevent->Type == MOUSEEVENT_BUTTONDOWN)
			{
				mButtonDown = true;
				g_pInput->m_EventList.pop_Event();
				return;
			}

			if(mouseevent->Type == MOUSEEVENT_BUTTONUP)
			{
				mButtonUp = true;
				g_pBehaviorEngine->m_EventList.add(mEvent);
				g_pInput->m_EventList.pop_Event();
				return;
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
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the button
	if(mEnabled)
	{
		CFont &Font = g_pGfxEngine->getFont(mFontID);
		Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, false );
		drawTwirl(lRect);
	}
	else
	{
		CFont &Font = g_pGfxEngine->getFont(0);
		Font.drawFont( blitsfc, mText, lRect.x+24, lRect.y, true );
	}
}



void CGUIButton::drawGalaxyBorderedStyle(SDL_Rect& lRect)
{
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	// Now lets draw the text of the list control
	CFont &Font = g_pGfxEngine->getFont(mFontID);

	SDL_PixelFormat *format = g_pVideoDriver->getBlitSurface()->format;

	const Uint32 oldcolor = Font.getFGColor();

	Uint32 newcolor;

	if(!mEnabled)
		newcolor = SDL_MapRGB( format, 123, 150, 123);
	else if(mHovered || mButtonDown)
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
	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

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
	if(!mEnabled)
		return;

	SDL_Surface *blitsfc = g_pVideoDriver->getBlitSurface();

	if( mButtonUp )
	{
		drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFBFBFBF );
	}
	else if( mButtonDown )
	{
		drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFCFCFCF );
	}
	else if( mHovered )
	{
		drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFDFDFDF );
	}
	else
	{
		drawRect( blitsfc, &lRect, 1, 0xFFBBBBBB, 0xFFFFFFFF );
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
